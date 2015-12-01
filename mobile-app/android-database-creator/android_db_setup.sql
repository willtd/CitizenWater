DROP TABLE if exists ReadingTypes;
DROP TABLE if exists Readings;
DROP TABLE if exists Devices;

CREATE TABLE ReadingTypes (
	Name TEXT, 
	Unit TEXT,
    ReadingTypeID INTEGER PRIMARY KEY);

CREATE TABLE Devices (
	DeviceCloudID TEXT, 
	DeviceName TEXT,
	DeviceCloudKey TEXT,
    DeviceID INTEGER PRIMARY KEY);

CREATE TABLE Readings (
    ReadingID INTEGER PRIMARY KEY,
	ReadingTypeID INTEGER, 
	Time LONG, 
	Value TEXT,
	DeviceID INTEGER,
	FOREIGN KEY(DeviceID) REFERENCES Devices(DeviceID),
	FOREIGN KEY(ReadingTypeID) REFERENCES ReadingTypes(ReadingTypeID));

INSERT INTO ReadingTypes VALUES
    ('pH','nil',1);
INSERT INTO ReadingTypes VALUES
    ('Conductivity','S',2);
INSERT INTO ReadingTypes VALUES
    ('Dissolved Oxygen','ppm',3);

INSERT INTO Readings VALUES 
	(1, 1, 1448080666, '6.5', 1);

INSERT INTO Readings VALUES 
	(2, 1, 1448080670, '6.0', 1);

INSERT INTO Readings VALUES 
	(3, 1, 1448111111, '6.2', 1);

INSERT INTO Readings VALUES 
	(4, 1, 1448152345 , '6.4', 1);

INSERT INTO Readings VALUES 
	(5, 1, 1448180100, '6.8', 1);

INSERT INTO Readings VALUES 
	(6, 1, 1448211706, '6.5', 1);

INSERT INTO Devices VALUES
    ('[Insert Device ID here]','Device 1','[Insert Device Key Here]',1);
INSERT INTO Devices VALUES
    ('[Insert Device ID here]','Device 2','[Insert Device Key Here]',2);
INSERT INTO Devices VALUES
    ('[Insert Device ID here]','Device 3','[Insert Device Key Here]',3);

