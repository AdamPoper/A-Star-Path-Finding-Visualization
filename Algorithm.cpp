#include "Algorithm.h"

const sf::Vector2f Node::s_nodeSize = { WIDTH / (float)ROWS, HEIGHT / (float)COLUMNS };
uint32_t Node::s_nodeCount = (ROWS * COLUMNS);

bool isBlocked(node_t* n, std::vector<node_t>& nodes)
{
    sf::Vector2i pos = { (int)n->Quad().getPosition().x, (int)n->Quad().getPosition().y };
    node_t* temp = Node::findNode(nodes, pos, COLUMNS, ROWS);
    if (temp->isBlocked())
        return true;
    else
        return false;
}

void findNeighbors(std::vector<node_t>& nodes, std::vector<node_t*>& open, node_t* currentNode, std::vector<node_t*>& closed, node_t* start, node_t* end)
{
    sf::Vector2f pos = currentNode->Quad().getPosition();
    std::vector<node_t*> children;
    children.reserve(4);
    node_t* child;
    {
        // North
        child = Node::findNode(nodes, { (int)pos.x, int(pos.y - Node::s_nodeSize.y) }, COLUMNS, ROWS);
        if (child != nullptr && !isBlocked(child, nodes))
        {
            child->g_costCalc(currentNode, Node::s_nodeSize.y);
            child->h_costCalc(end);
            child->f_costCalc();
            children.push_back(child);
        }
        // East
        child = Node::findNode(nodes, { (int)(pos.x + Node::s_nodeSize.x), (int)(pos.y) }, COLUMNS, ROWS);
        if (child != nullptr && !isBlocked(child, nodes))
        {
            child->g_costCalc(currentNode, Node::s_nodeSize.x);
            child->h_costCalc(end);
            child->f_costCalc();
            children.push_back(child);
        }
        // South
        child = Node::findNode(nodes, { (int)(pos.x), (int)(pos.y + Node::s_nodeSize.y) }, COLUMNS, ROWS);
        if (child != nullptr && !isBlocked(child, nodes))
        {
            child->g_costCalc(currentNode, Node::s_nodeSize.y);
            child->h_costCalc(end);
            child->f_costCalc();
            children.push_back(child);
        }
        // West
        child = Node::findNode(nodes, { (int)(pos.x - Node::s_nodeSize.x), (int)(pos.y) }, COLUMNS, ROWS);
        if (child != nullptr && !isBlocked(child, nodes))
        {
            child->g_costCalc(currentNode, Node::s_nodeSize.x);
            child->h_costCalc(end);
            child->f_costCalc();
            children.push_back(child);
        }
    }
    for (auto& c : children)
    {
        bool flag = false;
        for (auto& n : closed)
        {
            if (c == n)
            {
                flag = true;
                break;
            }
        }
        if (flag)
            continue;
        flag = false;

        for (auto& o : open)
        {
            if (c == o)
            {
                if (c->g_costGet() < o->g_costGet())
                {
                    flag = true;
                    break;
                }
            }
        }
        if (flag)
            continue;
        if (!flag)
        {
            c->Quad().setFillColor(sf::Color::Green);
            c->setParent(currentNode);
            open.push_back(c);
        }
    }
}
template<typename T>
void swap(T& a, T& b)
{
    T temp = a;
    a = b;
    b = temp;
}
void selectionSort(std::vector<node_t*>& open)
{
    int min_index = 0;
    for (int i = 0; i < open.size() - 1; i++)
    {
        min_index = i;
        for (int j = i + 1; j < open.size(); j++)
            if (*open[j] < *open[min_index])
                min_index = j;
        swap<node_t*>(open[min_index], open[i]);
    }
}