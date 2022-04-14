#include "config.h"

#include <features.h>
#include <gtkmm.h>

#include "Gchart.hpp"

class MyWindow : public Gtk::Window
{
	Gchart chart;
public:
	MyWindow();
};

MyWindow::MyWindow()
{
	set_title("Basic application");
	set_default_size(700, 700);
	std::map<const float, const float> map1 = {{0,0},{1,10},{2,10},{3,15},{4,5},{5,10},{5,11},{6,0}};
	std::map<const float, const float> map2 = {{0,100},{1.1,120},{1.5,140},{2,150},{3.1,110},{3.8,80},{4.5,91},{5.1,95},{5.5,105},{6,100}};

	this->chart.setLabels ("afstand", "km", nullptr, "snelheid", "km/u", nullptr, "hoogte", "m", nullptr);
	this->chart.addY1Chart (GchartChart::Type::LINEAR, 1, {1, 0, 0} , map1, nullptr);
	this->chart.addY2Chart (GchartChart::Type::LINEAR, 1, {0, 1, 0} , map2, nullptr);
	set_child (chart);
}

int main(int argc, char* argv[])
{
	auto app = Gtk::Application::create("org.gtkmm.examples");

	return app->make_window_and_run<MyWindow>(argc, argv);
}