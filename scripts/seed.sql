CREATE TEMP TABLE IF NOT EXISTS Variables (Name TEXT PRIMARY KEY, Value TEXT);

-- Employer one
INSERT INTO employers (name, is_active) VALUES ('MyCompany (Pty) Ltd', 1);
INSERT OR REPLACE INTO Variables ('myCompany', (SELECT last_insert_rowid()));

-- Client for employer one
INSERT INTO clients (name, is_active, employer_id) VALUES ('MyInsurer', 1, (SELECT Value FROM Variables WHERE Name = 'myCompany'));
INSERT OR REPLACE INTO Variables ('myInsurer', (SELECT last_insert_rowid()));

-- Project 1 for client 1
INSERT INTO projects (name, is_active, project_id, client_id) VALUES ('InsurerFrontEnd', 1, (SELECT Value FROM Variables WHERE Name = 'myCompany'), (SELECT Value FROM Variables WHERE Name = 'myInsurer'));
INSERT OR REPLACE INTO Variables ('insurerFrontEnd', (SELECT last_insert_rowid()));

-- Project 2 for client 1
INSERT INTO projects (name, is_active, project_id, client_id) VALUES ('InsurerBackEnd', 1, (SELECT Value FROM Variables WHERE Name = 'myCompany'), (SELECT Value FROM Variables WHERE Name = 'myInsurer'));
INSERT OR REPLACE INTO Variables ('insurerBackEnd', (SELECT last_insert_rowid()));

INSERT INTO tasks (task_date, is_active) VALUES (date(now), 1);
INSERT OR REPLACE INTO Variables ('taskDate', (SELECT last_insert_rowid()));

INSERT INTO categories (name, description, is_active, project_id) VALUES ('Programming', 'The task of inputting code into the machine to achieve a business objective', (SELECT Value FROM Variables WHERE Name = 'insurerFrontEnd'));
INSERT OR REPLACE INTO Variables ('programming', (SELECT last_insert_rowid()));

INSERT INTO categories (name, description, is_active, project_id) VALUES ('Analysis', 'Analysing new batch of work and gathering research before starting coding', (SELECT Value FROM Variables WHERE Name = 'insurerFrontEnd'));
INSERT OR REPLACE INTO Variables ('analysis', (SELECT last_insert_rowid()));

INSERT INTO task_details(start_time, end_time, duration, description, is_active, project_id, task_id, category_id) VALUES('06:15:00', '07:00:00', '00:45:00', 'Conducting analysis of reusing Angular component for user form input and emailing back client', 1, (SELECT Value FROM Variables WHERE Name = 'insurerFrontEnd'), (SELECT Value FROM Variables WHERE Name = 'taskDate'), (SELECT Value FROM Variables WHERE Name = 'analysis'));

-- Other
INSERT INTO employers (name, is_active) VALUES ('PersonalCompany Ltd', 1);
