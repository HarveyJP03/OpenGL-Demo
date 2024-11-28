#include "app.hpp"
#include "startPoint.hpp"
#include "Lab1.hpp"
#include "Lab2.hpp"



App::App(const WindowProperties& winProps) : Application(winProps)
{
	//m_layer = std::unique_ptr<Layer>(new StartPoint(m_window));
	//m_layer = std::unique_ptr<Layer>(new Lab1(m_window));
	m_layer = std::unique_ptr<Layer>(new Lab2(m_window));
}

Application* startApplication()
{
	WindowProperties props("GAMR3521", 800, 600);
	props.isHostingImGui = true;
	props.isResizable = false;
	return new App(props);
}