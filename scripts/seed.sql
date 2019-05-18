CREATE TEMP TABLE IF NOT EXISTS Variables (Name TEXT PRIMARY KEY, Value TEXT);

INSERT INTO employers (name, is_active) VALUES ('MyCompany (Pty) Ltd', 1);

INSERT OR REPLACE INTO Variables ('myCompany', (SELECT last_insert_rowid()));

INSERT INTO clients (name, is_active, employer_id) VALUES ('MyInsurer', 1, (SELECT Value FROM Variables WHERE Name = 'myCompany'));
INSERT OR REPLACE INTO Variables ('myInsurer', (SELECT last_insert_rowid()));

INSERT INTO projects (name, is_active, client_id) VALUES ('InsurerFrontEnd', 1, (SELECT Value FROM Variables WHERE Name = 'myInsurer'))
INSERT OR REPLACE INTO Variables ('insurerFrontEnd', (SELECT last_insert_rowid()));

-- Other
INSERT INTO employers (name, is_active) VALUES ('PersonalCompany Ltd', 1);
