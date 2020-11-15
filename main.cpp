#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include <list>
#include <iostream>
#include <thread>
#include <chrono>

#include "Node.h"

#define ROWS    50
#define COLUMNS 52

#define WIDTH   900
#define HEIGHT  900

const sf::Vector2f Node::s_nodeSize = { WIDTH / ROWS, HEIGHT / COLUMNS };
uint32_t Node::s_nodeCount = (ROWS * COLUMNS);
void initNodes(std::vector<node_t>& nodes)
{
    for (int y = 0; y < ROWS; y++)
        for (int x = 0; x < COLUMNS; x++)
            nodes.emplace_back(node_t(sf::Vector2f(x * Node::s_nodeSize.x, y * Node::s_nodeSize.y)));
}

node_t* pickStart(std::vector<node_t>& nodes, sf::Window& win)
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        node_t* start = Node::findNode(nodes, sf::Mouse::getPosition(win), COLUMNS, ROWS);
        if (start != nullptr)
        {
            start->Quad().setFillColor(sf::Color::Blue);
            return start;
        }
        else return nullptr;
    }
    return nullptr;
}
node_t* pickEnd(std::vector<node_t>& nodes, sf::Window& win)
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        node_t* end = Node::findNode(nodes, sf::Mouse::getPosition(win), COLUMNS, ROWS);
        if (end != nullptr)
        {
            end->Quad().setFillColor(sf::Color::Red);
            return end;
        }
        else return nullptr;
    }
    return nullptr;
}
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
        child = Node::findNode(nodes, { (int)pos.x, int(pos.y-Node::s_nodeSize.y) }, COLUMNS, ROWS);
        if (child != nullptr && !isBlocked(child, nodes))
        {
            //child->setParent(currentNode);
            children.push_back(child);       
        }
        // East
        child = Node::findNode(nodes, { (int)(pos.x + Node::s_nodeSize.x), (int)(pos.y) }, COLUMNS, ROWS);
        if (child != nullptr && !isBlocked(child, nodes))
        {
            //child->setParent(currentNode);
            children.push_back(child);               
        }
        // South
        child = Node::findNode(nodes, { (int)(pos.x), (int)(pos.y + Node::s_nodeSize.y) }, COLUMNS, ROWS);
        if (child != nullptr && !isBlocked(child, nodes))
        {
            //child->setParent(currentNode);
            children.push_back(child);       
        }
        // West
        child = Node::findNode(nodes, { (int)(pos.x - Node::s_nodeSize.x), (int)(pos.y) }, COLUMNS, ROWS);
        if (child != nullptr && !isBlocked(child, nodes))
        {
           // child->setParent(currentNode);
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
        c->g_costCalc(start);
        c->h_costCalc(end);
        c->f_costCalc();
        for (auto& o : open)
        {
            if (c == o)
            {
                if(c->g_costGet() < o->g_costGet())
                {
                    flag = true;
                    break;
                }                
            }
        }
        if (flag)
            continue;       
        c->Quad().setFillColor(sf::Color::Green);
        c->setParent(currentNode);
        open.push_back(c);
    }
}
template<typename T>
void swap(T& a, T& b)
{
    T temp = a;
    a = b;
    b = temp;
}
void bubbleSort(std::vector<node_t*>& open)
{
    for (int i = 0; i < open.size(); i++)
        for (int j = 0; j < open.size(); j++)
            if (*open[i] < *open[j])
                swap<node_t*>(open[i], open[j]);
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
int main()
{
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "A* Searching");
    Node::loadFont();
    std::vector<node_t> nodes;  // only using vector because it is too much memory to allocate on the stack
    nodes.reserve(ROWS * COLUMNS);
    initNodes(nodes);
    node_t* startNode   = nullptr;
    node_t * endNode    = nullptr;
    node_t* currentNode = nullptr;
    sf::Clock nodeClock;
    float startTime = 0.0f;
    std::vector<node_t*> open;         // nodes to be evaluated
    std::vector<node_t*> closed;       // nodes already evaluated
    std::vector<node_t*> blockedNodes; // nodes that are blocked off
    std::vector<node_t*> path;
    // display the initialized nodes
    window.clear();
    for (int i = 0; i < nodes.size(); i++)
        window.draw(nodes[i].Quad());
    window.display();
    // wait to get the start node
    while (!startNode)
    {
        startNode = pickStart(nodes, window);
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        for (int i = 0; i < nodes.size(); i++)
            window.draw(nodes[i].Quad());
        window.display();
    }    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));   
    // wait to get the end node    
    while (!endNode)
    {
        endNode = pickEnd(nodes, window);
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        for (int i = 0; i < nodes.size(); i++)
            window.draw(nodes[i].Quad());
        window.display();
    }    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    bool started = false;
    // set blocks
    while (!started)
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            node_t* n = Node::findNode(nodes, sf::Mouse::getPosition(window), COLUMNS, ROWS);
            if(n != nullptr)
                n->setBlocked();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            started = true;
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        for (int i = 0; i < nodes.size(); i++)
            nodes[i].draw(window);
        window.display();
    }
    bool found = false;
    sf::Clock startClock;
    open.push_back(startNode);
    currentNode = open[0];
    currentNode->reset_costs();
    currentNode->g_costCalc(startNode);
    currentNode->h_costCalc(endNode);
    currentNode->f_costCalc();
    while (window.isOpen())
    {                         
        if (!found && sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && startClock.getElapsedTime().asMilliseconds() >= 0.0f)
        {        
            startClock.restart();
            //std::cout << "Closed node count: " << closed.size() << std::endl;
            //std::this_thread::sleep_for(std::chrono::milliseconds(0));  
            //std::cout << "Sorting" << std::endl;
            selectionSort(open);
            //std::cout << "Sorted" << std::endl;
            currentNode = open[0];
            //std::cout << "Current Node Assigned to Lowest F" << std::endl;
            for (int i = 0; i < open.size(); i++)
                if (open[i]->h_costGet() < currentNode->h_costGet())
                    currentNode = open[i];

            for(auto it = open.begin(); it != open.end(); it++)
                if ((*it)->h_costGet() < currentNode->h_costGet())
                    currentNode = *it;                    
               
            ///std::cout << "Current Node Reassigned to lowest h among the lowest f" << std::endl;
            currentNode->Quad().setFillColor(sf::Color::Magenta);    
            //std::cout << "Erased Current Node from open" << std::endl;
            for (auto it = open.begin(); it != open.end(); it++)
            {
                if (*it == currentNode)
                {
                    open.erase(it);
                    break;
                }
            }
            closed.push_back(currentNode);
            //std::cout << "Included Current Node in closed" << std::endl;
            // removes duplicates cause there are for some reason
            for (auto openit = open.begin(); openit != open.end(); openit++)
            {
                for (auto closedit = closed.begin(); closedit != closed.end(); closedit++)
                    if (*openit == *closedit)
                    {
                        //std::cout << "before duplicate erase" << std::endl;                        
                        open.erase(openit);                          
                        //closed.erase(closedit);
                        //std::cout << "after duplicate erase" << std::endl;
                    }
                if (openit == open.end())
                    break;
            }
            //std::cout << "Removed any duplicates" << std::endl;
            if (currentNode == endNode)
                found = true;                                       
            if (!found)
                findNeighbors(nodes, open, currentNode, closed, startNode, endNode);    
            //std::cout << "Found neighbors" << std::endl;
        }
        window.clear();
        for (int i = 0; i < nodes.size(); i++)
            nodes[i].draw(window);
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        for (node_t* n : open)
        {
           // n->drawText(window);
            n->Quad().setFillColor(sf::Color::Green);            
        }       
        for (node_t* n : closed)
        {
            //n->drawText(window);
            n->Quad().setFillColor(sf::Color::Cyan);
        }
        if (currentNode != nullptr)
        {
            //currentNode->drawText(window);
            currentNode->Quad().setFillColor(sf::Color::Magenta);
            //std::cout << currentNode->getParent() << std::endl;
        }
        if (currentNode != nullptr && currentNode->getParent() != nullptr)
        {
            currentNode->getParent()->Quad().setFillColor(sf::Color::Yellow);
            path.push_back(currentNode->getParent());
        }
        if (found)
        {
            for (int i = 0; i < path.size(); i++)
                path[i]->Quad().setFillColor(sf::Color::Yellow);
        }
        
        window.display();        
    }

    return 0;
}