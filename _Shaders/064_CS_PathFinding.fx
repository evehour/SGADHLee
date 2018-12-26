#define ThreadX 32
#define ThreadY 32

const int NODE_NULL = 0;
const int NODE_OPEN = 1;
const int NODE_CLOSE = 2;

const int WALKABLE = 0;
const int COLLISION = 1;

int GridBlockCountX;
int MapDimensions;

struct Agent
{
    uint Id;
    uint2 Start;
    uint2 Target;
};

StructuredBuffer<Agent> AgentList;

/*
 그리드와 셀의 갯수는 일치할 필요가 없다.
 
*/
struct GridNode
{
    uint X;
    uint Y;
    uint ParentID;
    uint Status;
    uint Cost;
    uint GoalCost;
    uint Id;
};
RWStructuredBuffer<GridNode> GridNodeList;
// 간선들의 대한 정보
RWStructuredBuffer<uint> OpenList;


struct Search
{
    uint AgentId;
    uint FinalCost;
    uint2 Target;
};

void Insert(uint offset, uint x, uint y, uint currentCost, uint goalCost, uint parentId, uint status)
{
    uint currentSize = OpenList[offset]; // 갈수 있느냐 없느냐가 들어가있음.
    uint i = currentSize + 1;

    uint parentCost = GridNodeList[offset + OpenList[offset + (1 / 2)]].Cost; // 부모의 코스트를 받아오는 것.

    // 부모와 자식간에 비교해서 최소를 위로 올리는 것.
    [allow_uav_condition]
    while(i > 1 && parentCost > currentCost)
    {
        OpenList[offset + i] = OpenList[offset + (i / 2)];
        GridNodeList[offset + OpenList[offset + (i / 2)]].Id = i;

        i = i / 2;
        parentCost = GridNodeList[offset + OpenList[offset + i]].Cost;
    }

    GridNode node;
    node.X = x;
    node.Y = y;
    node.Cost = currentCost;
    node.ParentID = offset + parentId;
    node.GoalCost = goalCost;
    node.Id = i;
    uint gridId = MapDimensions * y + x;
    GridNodeList[offset + gridId] = node;
    
    OpenList[offset + i] = gridId;
    OpenList[offset + 0] = OpenList[offset] + 1;
}

void Update(uint offset, uint id, uint currentCost)
{
    uint i = id;

    uint originValue = OpenList[offset + i];
    uint parentCost = GridNodeList[offset + OpenList[offset + (i / 2)]].Cost;

    [allow_uav_condition]
    while(i > 1 && parentCost > currentCost)
    {
        OpenList[offset + i] = OpenList[offset + (i / 2)];
        GridNodeList[offset + OpenList[offset + i]].Id = i;

        i = i / 2;
        parentCost = GridNodeList[offset + OpenList[offset + i]].Cost; // 부모랑 비교하는 것.
    }
    
    OpenList[offset + i] = originValue;
    GridNodeList[offset + OpenList[offset + i]].Id + i;
}

void AddToOpenList(uint offset, uint2 node, GridNode parentGridNode, uint parentGridId, uint2 targetNode, uint goalCost)
{
    uint gridNodeId = MapDimensions * node.y + node.x;
    GridNode gridNode = GridNodeList[offset + gridNodeId];

    uint currentGoalCost = parentGridNode.Cost + goalCost;
    
    int x = abs((int) node.x - (int) targetNode.x);
    int y = abs((int) node.y - (int) targetNode.y);

    int temp = max(x, y);
    uint F = currentGoalCost * uint(temp) * 10; // 10은 기본 코스트값.

    if(gridNode.Status == NODE_NULL)
    {
        Insert(offset, node.x, node.y, F, currentGoalCost, parentGridId, NODE_OPEN);
    }
    else if(gridNode.Status == NODE_OPEN)
    {
        currentGoalCost = parentGridNode.GoalCost + goalCost;

        [flatten]
        if(currentGoalCost < gridNode.GoalCost)
        {
            GridNodeList[offset + gridNodeId].Cost = F;

            Update(offset, gridNode.Id, F); // 가장 짧은길을 찾기 위해 갱신하는 부분
        }
    }
}

void Remove(uint offset)
{
    uint current = OpenList[offset];
    uint node = 0;

    if (current >= 1)
    {
        node = OpenList[offset + 1];
        OpenList[offset + 1] = OpenList[offset + current];

        GridNodeList[offset + OpenList[offset + 1]].Id = 1;
        OpenList[offset + current] = 0;

        current = current - 1;
        OpenList[offset] = current;

        uint i = 1;

        // 루트가 아니란 얘기. (루트가 0번을 놓는 경우) 즉, 중간에 어떤 무언가를 지웠을 때.
        if(current >= 1)
        {
            [allow_uav_condition]
            while (true)
            {
                uint leftChild = (i * 2) + 0;
                uint rightChild = (i * 2) + 1;

                uint replaceId = 1; // 교체될 id
                uint replaceCost = 0; // 교체될 비용

                if(rightChild >= current)
                {
                    if(leftChild >= current)
                        break;
                    else
                        replaceId = leftChild;

                    replaceCost = GridNodeList[offset + OpenList[offset + replaceId]].Cost;
                }
                else
                {
                    uint leftChildCost = GridNodeList[offset + OpenList[offset + leftChild]].Cost;
                    uint rightChildCost = GridNodeList[offset + OpenList[offset + rightChild]].Cost;

                    if(leftChildCost < rightChildCost)
                    {
                        replaceId = leftChild;
                        replaceCost = leftChildCost;
                    }
                    else
                    {
                        replaceId = rightChild;
                        replaceCost = rightChildCost;
                    }
                }

                uint currentNodeCost = GridNodeList[offset + OpenList[offset + i]].Cost;
                if(currentNodeCost > replaceCost)
                {
                    uint temp = OpenList[offset + replaceId];
                    OpenList[offset + replaceId] = OpenList[offset + i];
                    OpenList[offset + i] = temp;

                    GridNodeList[offset + OpenList[offset + replaceId]].Id = replaceId;
                }
            }//while(true)

        }//if(current)
    }
}

[numthread(ThreadX, ThreadY, 1)]
void CS(uint3 groupThreadId : SV_GroupThreadID, uint3 dispatchThreadId : SV_DispatchThreadID, uint3 groupId : SV_GroupID)
{
    uint gridId = ((ThreadX * GridBlockCountX) * dispatchThreadId.y) + dispatchThreadId.x;
    uint offset = gridId * (MapDimensions * MapDimensions); // 한 칸당 건너뛸 범위.

    uint threadId = gridId;
    Agent agent = AgentList[threadId];

    uint2 start = agent.Start;
    uint2 target = agent.Target;

    uint targetGridId = MapDimensions * target.y + target.x;
    Insert(offset, start.x, start.y, 10, 0, 0, NODE_OPEN); // 무조건 기본으로 시작값 넣는 것임.  start와 target기준으로 그리드에 정보를 넣을 것.

    // 찾기 시작
    bool bFound = false;
    GridNode currentNode;

    [allow_uav_condition]
    while(bFound == false)
    {
        uint tempGridId = Remove(offset);

        GridNodeList[offset + tempGridId].Status = NODE_CLOSE;
        currentNode = GridNodeList[offset + tempGridId];
        
        if (currentNode.Cost == 0 && tempGridId == targetGridId)
        {
            bFound = true;
            break;
        }
        else
        {
            uint tempX = currentNode.X;
            uint tempY = currentNode.Y;
            uint2 node = 0;

            //자기 자신 기준으로 오른쪽을 검사
            if (MapDimensions > (tempX + 1))
            {
                node = uint2(currentNode.X + 1, currentNode.Y);
                AddToOpenList(offset, node, currentNode, tempGridId, target, 10);
            }
            
            //자기 자신 기준 우측 하단
            if (MapDimensions > (tempX + 1) && (tempY - 1) >= 0)
            {
                node = uint2(currentNode.X + 1, currentNode.Y - 1);
                AddToOpenList(offset, node, currentNode, tempGridId, target, 14);
            }

            //자기 자신 기준 하단
            if ((tempY - 1) >= 0)
            {
                node = uint2(currentNode.X, currentNode.Y - 1);
                AddToOpenList(offset, node, currentNode, tempGridId, target, 10);
            }

            //자기 자신 기준 좌측 하단
            if ((tempX - 1) >= 0 && (tempY - 1) >= 0)
            {
                node = uint2(currentNode.X - 1, currentNode.Y - 1);
                AddToOpenList(offset, node, currentNode, tempGridId, target, 14);
            }

            //자기 자신 기준 좌측
            if ((tempX - 1)  >= 0)
            {
                node = uint2(currentNode.X - 1, currentNode.Y);
                AddToOpenList(offset, node, currentNode, tempGridId, target, 10);
            }

            //자기 자신 기준 좌측 상단
            if ((tempX - 1)  >= 0 && MapDimensions > (tempY + 1))
            {
                node = uint2(currentNode.X - 1, currentNode.Y + 1);
                AddToOpenList(offset, node, currentNode, tempGridId, target, 14);
            }

            //자기 자신 기준 상단
            if (MapDimensions > (tempY + 1))
            {
                node = uint2(currentNode.X, currentNode.Y + 1);
                AddToOpenList(offset, node, currentNode, tempGridId, target, 10);
            }

            //자기 자신 기준 우측 상단
            if (MapDimensions > (tempX + 1) && MapDimensions > (tempY + 1))
            {
                node = uint2(currentNode.X + 1, currentNode.Y + 1);
                AddToOpenList(offset, node, currentNode, tempGridId, target, 10);
            }
        }
    }
    Search search;
    search.AgentId = agent.Id;
    search.FinalCost = currentNode.Cost;
    search.Target = float2(currentNode.X, currentNode.Y);
    SearchList[threadId] = search;
}


