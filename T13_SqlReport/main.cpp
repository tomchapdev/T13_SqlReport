#include <assert.h>
#include <iostream>
#include <windows.h>
#include <sstream>

#include "Utils.h"
#include "myDB.h"


using namespace std;

int main()
{
	void TablesInChinook(MyDB db);
	void FieldsInTable(MyDB db, string table);
	void CustomerInfoForSurname(MyDB db, string name);
	void MoneySpentBySurname(MyDB db, string name);
	void MoneySpentByAllCustomers(MyDB db);

	MyDB mydb;
	mydb.Init("chinook.db");

	TablesInChinook(mydb);
	FieldsInTable(mydb, "invoices");
	CustomerInfoForSurname(mydb, "'Gruber'");
	MoneySpentBySurname(mydb, "'Gruber'");
	MoneySpentByAllCustomers(mydb);

	mydb.Close();
	
	/*void ShowTableNames(sqlite3 * pdb);

	sqlite3* db;
	int rc = sqlite3_open("chinook.db", &db);
	if (rc)
	{
		DebugPrint("Can't open database: ", sqlite3_errmsg(db));
		assert(false);
	}
	else
		DebugPrint("Opened chinook");

	ShowTableNames(db);

	sqlite3_close(db);*/

	
}

//Prints the tables in chinook database
void TablesInChinook(MyDB db)
{
	db.ExecQuery("SELECT tbl_name FROM sqlite_master WHERE type = 'table'");
	DebugPrint("\nTables in Chinook");
	for (size_t i = 0; i < db.results.size(); ++i)
	{
		MyDB::Row r = db.results[i];
		for (size_t j = 0; j < r.size(); j++)
		{
			DebugPrint(r[j].value);
		}
	}
}

//Prints the fields in a given table
void FieldsInTable(MyDB db, string table)
{
	db.ExecQuery("SELECT * FROM " + table);
	DebugPrint("\nFields in " + table);
	for (size_t i = 0; i < db.results[0].size(); ++i)
	{
			DebugPrint(db.results[0][i].name);
	}
}

//Prints the customer info of a customer with given surname
void CustomerInfoForSurname(MyDB db, string name)
{
	db.ExecQuery("SELECT * FROM 'customers' WHERE LastName = " + name);
	DebugPrint("\nCustomer info for " + name);
	for (size_t i = 0; i < db.results.size(); i++)
	{
		DebugPrint("Customer Info:");
		for (size_t j = 0; j < db.results[i].size(); j++)
		{
			string str = db.results[i][j].name + ": " + db.results[i][j].value;
			DebugPrint(str);
		}
	}
}

//Prints the money spent by a given surname, listed by date and total
void MoneySpentBySurname(MyDB db, string name)
{
	db.ExecQuery("SELECT Total, InvoiceDate FROM customers INNER JOIN invoices ON customers.CustomerId = invoices.CustomerId WHERE LastName = " + name);
	DebugPrint("\nMoney spent by " + name);
	for (size_t i = 0; i < db.results.size(); i++)
	{
		string str = "£" + db.results[i][0].value + " - " + db.results[i][1].value;
		DebugPrint(str);
	}
	db.ExecQuery("SELECT SUM(Total) FROM customers INNER JOIN invoices ON customers.CustomerId = invoices.CustomerId WHERE LastName = " + name);
	string total = db.results[0][0].value;
	DebugPrint("Final total=£" + total);
}

//Prints the 
void MoneySpentByAllCustomers(MyDB db)
{
	db.ExecQuery("SELECT SUM(Total) TotalSpent, (FirstName || ' ' || LastName) AS Name FROM customers INNER JOIN invoices ON customers.CustomerId = invoices.CustomerId GROUP BY Name ORDER BY TotalSpent DESC");
	DebugPrint("\nHow much each customer has been charged");
	for (size_t i = 0; i < db.results.size(); i++)
	{
		string str = "£" + db.results[i][0].value + " - " + db.results[i][1].value;
		DebugPrint(str);
	}
	db.ExecQuery("SELECT SUM(Total) FROM invoices");
	int total = stof(db.results[0][0].value) * 100;
	string pounds = to_string(total / 100);
	string pence = to_string(total % 100);
	DebugPrint("Final total=£" + pounds + "." + pence);
}