#include <assert.h>
#include <iostream>
#include <sstream>

#include "SFML/Graphics.hpp"
#include "MyDB.h"
#include "Utils.h"

using namespace sf;
using namespace std;




int main()
{
	// Create the main window
	RenderWindow window(VideoMode(1200, 800), "database");
	sf::Font font;
	if (!font.loadFromFile("data/fonts/comic.ttf"))
		assert(false);

	MyDB mydb;
	mydb.Init("data/test.db");
	//check table exists
	mydb.ExecQuery("SELECT name FROM sqlite_master WHERE type = 'table' AND name = 'COMPANY'");
	if (mydb.results.empty())
	{
		mydb.ExecQuery("CREATE TABLE COMPANY("  \
				"ID INT PRIMARY KEY     NOT NULL," \
				"NAME           TEXT    NOT NULL," \
				"AGE            INT     NOT NULL," \
				"ADDRESS        CHAR(50)," \
				"SALARY         FLOAT )");
	}
	//check data exists
	mydb.ExecQuery("SELECT * FROM COMPANY");
	if (mydb.results.empty())
	{
		//empty so add it
		mydb.ExecQuery( "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  \
			"VALUES (1, 'Paul', 32, 'California', 20000.00 ); " \
			"INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  \
			"VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "     \
			"INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" \
			"VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );" \
			"INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" \
			"VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 )");
		mydb.ExecQuery("SELECT * FROM COMPANY");
	}

	//how many have a higher wage?
	mydb.ExecQuery("SELECT COUNT(*) AS 'numHighEarners' FROM COMPANY WHERE SALARY > 20000");
	stringstream ss;
	assert(mydb.results.size() == 1);
	ss << "Num high earners=" << mydb.results[0][0].value;
	DebugPrint(ss.str(), "");

	//get details for young and old staff
	DebugPrint("Senior staff:");
	mydb.ExecQuery("SELECT * FROM COMPANY WHERE AGE>=25");
	for (size_t i = 0; i < mydb.results.size(); ++i)
		DebugPrint(mydb.GetStr(i, "NAME"));
	DebugPrint("Junior staff:");
	mydb.ExecQuery("SELECT * FROM COMPANY WHERE AGE<25");
	for (size_t i = 0; i < mydb.results.size(); ++i)
		DebugPrint(mydb.GetStr(i, "NAME"));

	//ordered list of how earns the most
	DebugPrint("Salary high scores:");
	mydb.ExecQuery("SELECT ID, NAME, SALARY FROM COMPANY ORDER BY SALARY DESC");
	for (size_t i = 0; i < mydb.results.size(); ++i) {
		DebugPrint(mydb.GetStr(i, "NAME") + ":" + mydb.GetStr(i, "SALARY"));
	}
	//give the poorest member a rise
	int lastRec = mydb.results.size() - 1;
	DebugPrint("Let's give " + mydb.GetStr(lastRec, "NAME") + " a £10k pay rise");
	ss.str("");
	float salary = mydb.GetFloat(lastRec, "SALARY");
	int ID = mydb.GetInt(lastRec, "ID");
	ss << "UPDATE COMPANY SET SALARY = " << salary + 10000 << " WHERE ID=" << ID;
	mydb.ExecQuery(ss.str());
	//redo the list
	DebugPrint("Salary high scores v2 - fair pay:");
	mydb.ExecQuery("SELECT NAME, SALARY FROM COMPANY ORDER BY SALARY DESC");
	for (size_t i = 0; i < mydb.results.size(); ++i) {
		DebugPrint(mydb.GetStr(i, "NAME") + ":" + mydb.GetStr(i, "SALARY"));
	}

	mydb.SaveToDisk();
	mydb.Close();


	Clock clock;
	// Start the game loop 
	while (window.isOpen())
	{
		bool fire = false;
		// Process events
		Event event;
		while (window.pollEvent(event))
		{
			// Close window: exit
			if (event.type == Event::Closed) 
				window.close();
		} 

		// Clear screen
		window.clear();

		float elapsed = clock.getElapsedTime().asSeconds();
		clock.restart();

		sf::Text txt("Hello world", font, 30);
		window.draw(txt);


		// Update the window
		window.display();
	}

	return EXIT_SUCCESS;
}
