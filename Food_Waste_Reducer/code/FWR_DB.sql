-- Stergerea FWR_DB
DROP DATABASE IF EXISTS FWR_DB;

-- Crearea FWR_DB
CREATE DATABASE IF NOT EXISTS FWR_DB;
USE FWR_DB;

-- Tabelul client0
CREATE TABLE IF NOT EXISTS client0 (
    ID0 INT PRIMARY KEY AUTO_INCREMENT,
    Username VARCHAR(255) NOT NULL
);

-- Tabelul client1
CREATE TABLE IF NOT EXISTS client1 (
    ID1 INT PRIMARY KEY AUTO_INCREMENT,
    Username VARCHAR(255) NOT NULL
);

-- Tabelul Donations
CREATE TABLE IF NOT EXISTS Donations (
    ID_Donation INT PRIMARY KEY AUTO_INCREMENT,
    ID0 INT,
    ID1 INT,
    FOREIGN KEY (ID0) REFERENCES client0(ID0),
    FOREIGN KEY (ID1) REFERENCES client1(ID1)
);

-- Tabelul Products
CREATE TABLE IF NOT EXISTS Products (
    ID_Product INT PRIMARY KEY AUTO_INCREMENT,
    ID_Donation INT,
    Type VARCHAR(255) NOT NULL,
    Amount DECIMAL(10, 2) NOT NULL,
    FOREIGN KEY (ID_Donation) REFERENCES Donations(ID_Donation)
);

-- Inserare în tabelul client0
INSERT INTO FWR_DB.client0 (Username) VALUES ('Olaru'), ('Popescu'), ('Prunariu');

-- Inserare în tabelul client1
INSERT INTO FWR_DB.client1 (Username) VALUES ('Profi'), ('Mega'), ('Oscar');

-- Inserare în tabelul Donations
INSERT INTO FWR_DB.Donations (ID0, ID1) VALUES (1, 2), (3, 1), (2, 3);

-- Inserare în tabelul Products
INSERT INTO FWR_DB.Products (ID_Donation, Type, Amount) VALUES
    (1, 'TypeA', 50.00),
    (1, 'TypeB', 30.00),
    (2, 'TypeC', 25.00),
    (3, 'TypeA', 40.00);

SELECT * FROM FWR_DB.client0;
SELECT * FROM FWR_DB.client1;
SELECT * FROM FWR_DB.Donations;
SELECT * FROM FWR_DB.Products;

SELECT * FROM FWR_DB.Products WHERE ID_Donation = 1;