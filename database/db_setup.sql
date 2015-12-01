DROP TABLE if exists ReadingTypes;
DROP TABLE if exists Readings;

CREATE TABLE if not exists ReadingTypes (
	ReadingTypeID INTEGER primary key, 
	Name TEXT, 
	Unit TEXT);

CREATE TABLE if not exists Readings (
	ReadingID INTEGER primary key, 
	ReadingTypeID INTEGER, 
	Time INTEGER, 
	Value TEXT, 
	FOREIGN KEY(ReadingTypeID) REFERENCES ReadingTypes(ReadingTypeID));

INSERT INTO ReadingTypes VALUES(1, "Conductivity", "Unit?");
INSERT INTO ReadingTypes VALUES(2, "Dissolved Oxygen", "Unit?");
INSERT INTO ReadingTypes VALUES(3, "Oxygen Reduction Potential", "Unit?");
INSERT INTO ReadingTypes VALUES(4, "pH", "Unit?");

