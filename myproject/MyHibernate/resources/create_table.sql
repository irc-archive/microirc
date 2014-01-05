select * from UserPersonalized;

drop table UserPersonalized;

CREATE TABLE UserPersonalized
(
FName VARCHAR(100),
LName VARCHAR(100),
ID INT
);

ALTER TABLE UserPersonalized ADD COLUMN `author` int(10) 