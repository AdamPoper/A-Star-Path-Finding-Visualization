#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <sstream>
#include <iostream>

class Node;
typedef Node node_t;
class Node
{
public:
	Node(const sf::Vector2f& pos);
	sf::RectangleShape& Quad();
	static const sf::Vector2f s_nodeSize;
	static uint32_t s_nodeCount;
	static node_t* findNode(std::vector<node_t>& nodes, sf::Vector2i pos, uint32_t COLUMNS, uint32_t ROWS);
	void g_costCalc(node_t* start);
	void h_costCalc(node_t* end);
	void f_costCalc();
	float g_costGet() const;
	float h_costGet() const;
	float f_costGet() const;
	void setParent(node_t* parent);
	void reset_costs();
	bool isBlocked();
	void setBlocked();
	node_t* getParent() const;
	void draw(sf::RenderWindow& win);
	void drawText(sf::RenderWindow& win);
	static void loadFont();
	bool operator<(const Node& n);
	bool operator>(const Node& n);
	bool operator<=(const Node& n);
	bool operator>=(const Node& n);
private:	
	sf::RectangleShape m_quad;
	float g_cost, h_cost, f_cost;
	static sf::Font s_font;
	sf::Text g_text, h_text, f_text;
	std::string g_str, h_str, f_str;
	Node* m_parent = nullptr;
	bool is_blocked = false;
};