#include "Algorithm.h"

void initNodes(std::vector<node_t>& nodes)
{
    uint32_t node_id = 0;
    for (int y = 0; y < ROWS; y++)
    {
        for (int x = 0; x < COLUMNS; x++)
        {
            nodes.emplace_back(Node(sf::Vector2f(x * Node::s_nodeSize.x, y * Node::s_nodeSize.y)));
            nodes[node_id].setRenderID(node_id);
            node_id++;
        }
    }
}

node_t* pickNode(std::vector<node_t>& nodes, sf::Window& win, sf::Color col)
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        node_t* end = Node::findNode(nodes, sf::Mouse::getPosition(win), COLUMNS, ROWS);
        if (end != nullptr)
        {
            end->Quad().setFillColor(col);
            return end;
        }
        else return nullptr;
    }
    return nullptr;
}
node_t* pickNode(std::vector<node_t>& nodes, sf::RenderWindow& win)
{
    sf::Clock clock;
    node_t* n = nullptr;
    while (!n)
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && clock.getElapsedTime().asMilliseconds())
        {
            clock.restart();
            n = Node::findNode(nodes, sf::Mouse::getPosition(win), COLUMNS, ROWS);
        }
        sf::Event event;
        while (win.pollEvent(event))
            if (event.type == sf::Event::Closed)
                win.close();       
        win.clear();
        for (int i = 0; i < nodes.size(); i++)
            win.draw(nodes[i].Quad());
        win.display();
    }
    return n;
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
    // display the initialized nodes
    window.clear();
    for (int i = 0; i < nodes.size(); i++)
        window.draw(nodes[i].Quad());
    window.display();
    // wait to get the start node   
    startNode = pickNode(nodes, window);
    startNode->Quad().setFillColor(sf::Color::Blue);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));   
    // wait to get the end node    
    endNode = pickNode(nodes, window);
    endNode->Quad().setFillColor(sf::Color::Red);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
    open.push_back(startNode);
    currentNode = open[0];
    currentNode->reset_costs();
    currentNode->g_costCalc(startNode, 0.0f);
    currentNode->h_costCalc(endNode);
    currentNode->f_costCalc();
    while (window.isOpen())
    {                         
        if (!found)
        {        
            selectionSort(open);   // sort the nodes by lowest f cost
            currentNode = open[0]; // assign the current node to the open node with lowest f            
            for(auto it = open.begin(); it != open.end(); it++)
                if ((*it)->h_costGet() <= currentNode->h_costGet())
                    currentNode = *it;                  // among the lowest f cost nodes find the one with the lowest h                                  
            currentNode->Quad().setFillColor(sf::Color::Magenta); 
            // erase the currentNode from the open list
            for (auto it = open.begin(); it != open.end(); it++)
            {
                if (*it == currentNode)
                {
                    open.erase(it);
                    break;
                }
            }
            closed.push_back(currentNode);  // add the current node to the closed list
            // removes duplicates cause there are for some reason
            for (auto openit = open.begin(); openit != open.end(); openit++)
            {
                for (auto closedit = closed.begin(); closedit != closed.end(); closedit++)
                    if (*openit == *closedit)
                        open.erase(openit);                                              
                if (openit == open.end())
                    break;
            }
            if (currentNode == endNode)  // check if the target node has been found
                found = true;                                       
              // if the end node hasn't been found, find the new neighbors of the current node
            findNeighbors(nodes, open, currentNode, closed, startNode, endNode);    
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
            n->Quad().setFillColor(sf::Color::Green);                                               
        for (node_t* n : closed)
            n->Quad().setFillColor(sf::Color::Cyan);                        
        if (currentNode != nullptr)
            currentNode->Quad().setFillColor(sf::Color::Magenta);                                   
        if (found)
        {
            // retrace the parents to find out what the shortest path is
            node_t* n = endNode;
            while (n->getParent() != nullptr)
            {
                n = n->getParent();
                n->Quad().setFillColor(sf::Color::Yellow);
            }
            startNode->Quad().setFillColor(sf::Color::Blue);
            endNode->Quad().setFillColor(sf::Color::Red);
        }        
        window.display();        
    }

    return 0;
}