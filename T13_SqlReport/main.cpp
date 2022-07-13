#include <assert.h>
#include <iostream>
#include <windows.h>
#include <sstream>

#include "Utils.h"
#include "myDB.h"

using namespace std;

int main()
{
	void TablesInDatabase(MyDB db, const string& name);
	void FieldsInTable(MyDB db, const string & table);
	void CustomerInfo(MyDB db, const string& firstName, const string& lastName);
	void MoneySpentByCustomer(MyDB db, const string & firstName, const string & lastName);
	void MoneySpentByAllCustomers(MyDB db);

	MyDB mydb;
	mydb.Init("chinook.db");

	TablesInDatabase(mydb, "Chinook");
	FieldsInTable(mydb, "invoices");
	CustomerInfo(mydb, "Julia", "Barnett");
	MoneySpentByCustomer(mydb, "Julia", "Barnett");
	MoneySpentByAllCustomers(mydb);

	mydb.Close();
}

//Prints the tables in a given database
void TablesInDatabase(MyDB db, const string& name)
{
	db.ExecQuery("SELECT tbl_name FROM sqlite_master WHERE type = 'table'");
	DebugPrint("\nTables in " + name);

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
void FieldsInTable(MyDB db, const string& table)
{
	db.ExecQuery("SELECT * FROM " + table);
	DebugPrint("\nFields in " + table);

	for (size_t i = 0; i < db.results[0].size(); ++i)
	{
			DebugPrint(db.results[0][i].name);
	}
}

//Prints the customer info specified by a given first and last name
void CustomerInfo(MyDB db, const string& firstName, const string& lastName)
{
	db.ExecQuery("SELECT * FROM 'customers' WHERE FirstName = '" + firstName + "' AND LastName = '" + lastName + "'");
	DebugPrint("\nCustomer info for " + firstName + " " + lastName);

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

//Prints the money spent by a customer, listed by date and total
void MoneySpentByCustomer(MyDB db, const string& firstName, const string& lastName)
{
	db.ExecQuery("SELECT Total, InvoiceDate FROM customers INNER JOIN invoices ON customers.CustomerId = invoices.CustomerId " \
		"WHERE FirstName = '" + firstName + "' AND LastName = '" + lastName + "'");
	DebugPrint("\nMoney spent by " + firstName + " " + lastName);

	for (size_t i = 0; i < db.results.size(); i++)
	{
		string str = "£" + db.results[i][0].value + " - " + db.results[i][1].value;
		DebugPrint(str);
	}

	db.ExecQuery("SELECT SUM(Total) FROM customers INNER JOIN invoices ON customers.CustomerId = invoices.CustomerId " \
		"WHERE FirstName = '" + firstName + "' AND LastName = '" + lastName + "'");
	string total = db.results[0][0].value;
	DebugPrint("Final total = £" + total);
}

//Prints the total money spent by all customers
void MoneySpentByAllCustomers(MyDB db)
{
	db.ExecQuery("SELECT SUM(Total) TotalSpent, (FirstName || ' ' || LastName) AS Name " \
		"FROM customers INNER JOIN invoices ON customers.CustomerId = invoices.CustomerId GROUP BY Name ORDER BY TotalSpent DESC");
	DebugPrint("\nHow much each customer has been charged: ");

	for (size_t i = 0; i < db.results.size(); i++)
	{
		string str = "£" + db.results[i][0].value + " - " + db.results[i][1].value;
		DebugPrint(str);
	}

	db.ExecQuery("SELECT SUM(Total) FROM invoices");
	int total = stof(db.results[0][0].value) * 100;
	string pounds = to_string(total / 100);
	string pence = to_string(total % 100);
	DebugPrint("Final total = £" + pounds + "." + pence);
}