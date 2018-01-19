#if CCR_FORCE_LLVM_INTERPRETER
#error "Clang/LLVM interpreter does not support native to script callback function yet. Consider using CPP built-in compiler or turn on 'Use JIT execution' compiler option in app options menu."
#endif

#include <stdio.h>
#include <stdlib.h>

#include <sqlite3.h>

#ifndef NULL
#define NULL 0
#endif

#define DB_FILE "test.db"

int callback(void *parameter, int argc, char **argv, char **columnName);

int main()
{
	sqlite3 *db = NULL;
	char *errorMessage = NULL;

	//1. Open database
	if (sqlite3_open(DB_FILE, &db) == SQLITE_OK)
		printf("Database file opened : %s\n", DB_FILE);
	else
	{
		printf("Error openning database : %s\n",
			sqlite3_errmsg(db));
		return -1;
	}

	//2. Create table
	const char *sql = "CREATE TABLE INVENTORY("
		"NAME TEXT NOT NULL,"
		"NUMBER INT NOT NULL);";

	if (sqlite3_exec(db, sql, callback, 0, &errorMessage) == SQLITE_OK)
		printf("SQL execution complete : CREATE TABLE...\n");
	else
	{
		printf("Error executing SQL : %s\n", errorMessage);
		sqlite3_free(errorMessage);
		sqlite3_close(db);
		return -1;
	}

	//3. Insert record
	sql = "INSERT INTO INVENTORY (NAME,NUMBER) "
		"VALUES ('Apple',300); "
		"SELECT * from INVENTORY;";

	if (sqlite3_exec(db, sql, callback, 0, &errorMessage) == SQLITE_OK)
		printf("SQL execution complete : INSERT INTO...\n");
	else
	{
		printf("Error executing SQL : %s\n", errorMessage);
		sqlite3_free(errorMessage);
		sqlite3_close(db);
		return -1;
	}

	//4. View record
	sql = "SELECT * from INVENTORY;";

	if (sqlite3_exec(db, sql, callback, 0, &errorMessage) == SQLITE_OK)
		printf("SQL execution complete : SELECT...\n");
	else
	{
		printf("Error executing SQL : %s\n", errorMessage);
		sqlite3_free(errorMessage);
		sqlite3_close(db);
		return -1;
	}

	//5. Update record
	sql = "UPDATE INVENTORY set NUMBER=350 where NAME='Apple'; "
		"SELECT * from INVENTORY;";

	if (sqlite3_exec(db, sql, callback, 0, &errorMessage) == SQLITE_OK)
		printf("SQL execution complete : UPDATE...\n");
	else
	{
		printf("Error executing SQL : %s\n", errorMessage);
		sqlite3_free(errorMessage);
		sqlite3_close(db);
		return -1;
	}

	sqlite3_close(db);

	return 0;
}

int callback(void *parameter, int argc, char **argv, char **columnName)
{
	for (int i = 0; i < argc; i++)
		printf("%s : %s\n", columnName[i], argv[i]);

	return 0;
}
