#include "Node.h"

sf::Font Node::s_font;
void Node::loadFont()
{
	s_font.loadFromFile("assets/fonts/Arial.ttf");
}
Node::Node(const sf::Vector2f& pos)
{
	m_quad.setFillColor(sf::Color::White);
	m_quad.setOutlineColor(sf::Color::Black);
	m_quad.setOutlineThickness(2);
	m_quad.setPosition(pos);
	m_quad.setSize(s_nodeSize);
	//m_font.loadFromFile("assets/fonts/Arial.ttf");
	g_text.setCharacterSize(9);
	g_text.setPosition({ m_quad.getPosition().x + 1.0f, m_quad.getPosition().y + 5.0f });
	g_text.setFont(s_font);
	g_text.setFillColor(sf::Color::Black);
	h_text.setCharacterSize(9);
	h_text.setPosition({ m_quad.getPosition().x + 1.0f, m_quad.getPosition().y + 20.0f });
	h_text.setFont(s_font);
	h_text.setFillColor(sf::Color::Black);
	f_text.setCharacterSize(9);
	f_text.setPosition({ m_quad.getPosition().x + 1.0f, m_quad.getPosition().y + 40.0f });
	f_text.setFont(s_font);
	f_text.setFillColor(sf::Color::Black);
	g_str = "G: ";
	h_str = "H: ";
	f_str = "F: ";
	g_text.setString(g_str.c_str());
}
sf::RectangleShape& Node::Quad() { return m_quad; }

void Node::f_costCalc() 
{
	f_cost = h_cost + g_cost;
	f_str = "F: ";
	std::stringstream ss;
	ss << f_cost;
	std::string ft;
	ss >> ft;
	f_str += ft;
	f_text.setString(f_str.c_str());	
}
void Node::g_costCalc(node_t* start)
{
	float a = abs(start->Quad().getPosition().x - m_quad.getPosition().x);
	float b = abs(start->Quad().getPosition().y - m_quad.getPosition().y);
	float c = sqrt((a * a) + (b * b));
	g_cost = c;
	g_str = "G: ";
	std::stringstream ss;
	ss << g_cost;
	std::string gt;
	ss >> gt;
	g_str += gt;
	g_text.setString(g_str.c_str());	
}
void Node::h_costCalc(node_t* end)
{
	float a = abs(end->Quad().getPosition().x - m_quad.getPosition().x);
	float b = abs(end->Quad().getPosition().y - m_quad.getPosition().y);
	float c = sqrt((a * a) + (b * b));
	h_cost = c;
	h_str = "H: ";
	std::stringstream ss;
	ss << h_cost;
	std::string ht;
	ss >> ht;
	h_str += ht;
	h_text.setString(h_str.c_str());
}
node_t* Node::findNode(std::vector<node_t>& nodes, sf::Vector2i pos, uint32_t COLUMNS, uint32_t ROWS)
{
	if (pos.y < 0 || pos.y > Node::s_nodeSize.y * ROWS)
		return nullptr;
	else if (pos.x < 0 || pos.y > Node::s_nodeSize.x * COLUMNS)
		return nullptr;
	int col = pos.x / Node::s_nodeSize.x;
	int row = pos.y / Node::s_nodeSize.y;
	int x = row * COLUMNS;
	int y = x + col;
	node_t* n = nullptr;
	if (y < 0 || y >= s_nodeCount)
		return nullptr;
	else
	{			
		n = &nodes[y];		
		return n;
	}
}
void Node::draw(sf::RenderWindow& win)
{
	win.draw(m_quad);	
}
void Node::setParent(node_t* parent)
{
	m_parent = parent;
}
void Node::reset_costs() 
{
	f_cost = 0.0f; 
	g_cost = 0.0f;
	h_cost = 0.0f;
}
void Node::drawText(sf::RenderWindow& win)
{	
	g_text.setFont(s_font);	
	win.draw(g_text);	
	h_text.setFont(s_font);
	win.draw(h_text);
	f_text.setFont(s_font);
	win.draw(f_text);
}
float Node::g_costGet() const { return g_cost; }
float Node::h_costGet() const { return h_cost; }
float Node::f_costGet() const { return f_cost; }

bool Node::operator<(const Node& n)
{
	return f_cost < n.f_cost;
}
bool Node::operator>(const Node& n)
{
	return f_cost > n.f_cost;
}
bool Node::operator<=(const Node& n)
{
	return f_cost <= n.f_cost;
}
bool Node::operator>=(const Node& n)
{
	return f_cost >= n.f_cost;
}
void Node::setBlocked() 
{ 
	is_blocked = true; 
	m_quad.setFillColor(sf::Color::Black);
}
bool Node::isBlocked() { return is_blocked; }
node_t* Node::getParent() const
{
	return m_parent;
}