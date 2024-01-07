-- Stergerea FWR_DB
DROP DATABASE IF EXISTS FWR_DB;

-- Crearea FWR_DB
CREATE DATABASE IF NOT EXISTS FWR_DB;
USE FWR_DB;

-- Tabelul client0
CREATE TABLE IF NOT EXISTS FWR_DB.client0 (
    ID0 INT PRIMARY KEY AUTO_INCREMENT,
    Username VARCHAR(255) NOT NULL
);

-- Tabelul client1
CREATE TABLE IF NOT EXISTS FWR_DB.client1 (
    ID1 INT PRIMARY KEY AUTO_INCREMENT,
    Username VARCHAR(255) NOT NULL
);

-- Tabelul Donations
CREATE TABLE IF NOT EXISTS FWR_DB.Donations (
    ID_Donation INT PRIMARY KEY AUTO_INCREMENT,
    ID0 INT,
    ID1 INT,
    FOREIGN KEY (ID0) REFERENCES client0(ID0),
    FOREIGN KEY (ID1) REFERENCES client1(ID1)
);

-- Tabelul Products
CREATE TABLE IF NOT EXISTS FWR_DB.Products (
    ID_Product INT PRIMARY KEY AUTO_INCREMENT,
    ID_Donation INT,
    Type VARCHAR(255) NOT NULL,
    Amount DECIMAL(10, 2) NOT NULL,
    FOREIGN KEY (ID_Donation) REFERENCES Donations(ID_Donation)
);

-- Inserare în tabelul client0
INSERT INTO FWR_DB.client0 (Username) VALUES ('Olaru'), ('Popescu'), ('Prunariu'), ('Ionescu');

-- Inserare în tabelul client1
INSERT INTO FWR_DB.client1 (Username) VALUES ('Profi'), ('Mega'), ('Oscar'), ('Kaufland');

-- Inserare în tabelul Donations
INSERT INTO FWR_DB.Donations (ID0, ID1) VALUES (NULL, 1), (2, 2), (NULL, 1), (NULL, 2), (3, 1), (NULL, 2), (4, 3), (NULL, 4);

-- Inserare în tabelul Products
INSERT INTO FWR_DB.Products (ID_Donation, Type, Amount) VALUES
    (1, 'TypeB', 110.00),
    (2, 'TypeC', 50.00),
    (2, 'TypeA', 70.00),
    (3, 'TypeD', 10.00),
    (3, 'TypeA', 43.00),
    (4, 'TypeB', 100.00),
    (5, 'TypeC', 20.00),
    (6, 'TypeA', 30.00),
    (7, 'TypeD', 40.00),
    (8, 'TypeB', 50.00),
    (8, 'TypeA', 60.00),
    (8, 'TypeC', 70.00);

ALTER TABLE FWR_DB.Donations ADD COLUMN pending_request INT DEFAULT 0;

ALTER TABLE FWR_DB.Donations ADD COLUMN accepted_request INT DEFAULT -1;

SELECT * FROM FWR_DB.client0;
SELECT * FROM FWR_DB.client1;
SELECT * FROM FWR_DB.Donations;
SELECT * FROM FWR_DB.Products;

SELECT * FROM FWR_DB.Products WHERE ID_Donation = 1;

UPDATE FWR_DB.Donations SET FWR_DB.Donations.ID0 = 1 WHERE FWR_DB.Donations.ID_Donation IN (1,3,4,6,8);

UPDATE FWR_DB.Donations SET FWR_DB.Donations.ID0 = NULL WHERE FWR_DB.Donations.ID_Donation IN (1,3,4,6,8);


UPDATE FWR_DB.Donations SET FWR_DB.Donations.accepted_request = -1 WHERE FWR_DB.Donations.ID1 IN (1,2,3,4);

SELECT * FROM FWR_DB.Products JOIN FWR_DB.Donations ON FWR_DB.Products.ID_Donation = FWR_DB.Donations.ID_Donation WHERE FWR_DB.Donations.ID0 IS NULL
SELECT * FROM FWR_DB.Products p JOIN FWR_DB.Donations d ON p.ID_Donation = d.ID_Donation WHERE d.ID_Donation = 8 AND d.ID0 IS NULL

SELECT Username FROM FWR_DB.client0;

SELECT * FROM FWR_DB.Products JOIN FWR_DB.Donations ON FWR_DB.Products.ID_Donation = FWR_DB.Donations.ID_Donation JOIN FWR_DB.client0 ON FWR_DB.client0.ID0=FWR_DB.Donations.ID0 WHERE FWR_DB.Donations.ID0 IS NULL AND FWR_DB.client0.Username = 'Olaru';

SELECT * FROM FWR_DB.Products JOIN FWR_DB.Donations ON FWR_DB.Products.ID_Donation = FWR_DB.Donations.ID_Donation WHERE FWR_DB.Donations.ID0 IS NULL

SELECT * FROM FWR_DB.client0 WHERE FWR_DB.client0.Username = 'Olaru';

SELECT * FROM FWR_DB.Donations JOIN FWR_DB.client1 ON FWR_DB.Donations.ID1=FWR_DB.client1.ID1 WHERE ID0 IS NULL AND FWR_DB.client1.Username = 'Profi';

UPDATE FWR_DB.Donations SET FWR_DB.Donations.ID0 = 1 , FWR.DB.Donations.pending_request= 1 WHERE FWR_DB.Donations.ID_Donation = 1;

UPDATE FWR_DB.Donations SET FWR_DB.Donations.pending_request = 0 WHERE FWR_DB.Donations.ID1 IN (SELECT ID1 FROM FWR_DB.client1 WHERE Username = ' Mega ');
UPDATE FWR_DB.Donations SET FWR_DB.Donations.pending_request = 0, FWR_DB.Donations.ID0 = NULL  WHERE FWR_DB.Donations.ID1 IN (SELECT ID1 FROM FWR_DB.client1 WHERE Username = 'Mega');

SELECT * FROM FWR_DB.Donations JOIN FWR_DB.client0 ON FWR_DB.Donations.ID0=FWR_DB.client0.ID0 WHERE FWR_DB.Donations.accepted_request = 1 AND FWR_DB.client0.Username = 'Olaru';

UPDATE FWR_DB.Donations SET FWR_DB.Donations.pending_request = 0 , FWR_DB.Donations.accepted_request = 1 WHERE FWR_DB.Donations.pending_request = 1 AND FWR_DB.Donations.ID1 IN (SELECT ID1 FROM FWR_DB.client1 WHERE Username = 'Profi')