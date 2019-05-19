CREATE TEMP TABLE IF NOT EXISTS Variables (Name TEXT PRIMARY KEY, Value TEXT);

-- Employer one
INSERT INTO employers (name, is_active) VALUES ('MyCompany (Pty) Ltd', 1);
INSERT INTO Variables(Name, Value) SELECT 'myCompany', last_insert_rowid();

-- Client for employer one
INSERT INTO clients (name, is_active, employer_id) VALUES ('MyInsurer', 1, (SELECT Value FROM Variables WHERE Name = 'myCompany'));
INSERT INTO Variables(Name, Value) SELECT 'myInsurer', last_insert_rowid();

-- Project 1 for client 1
INSERT INTO projects (name, display_name, is_active, client_id, employer_id) VALUES ('InsurerFrontEnd', 'InsurerFrontEnd Modern Rewrite', 1, (SELECT Value FROM Variables WHERE Name = 'myCompany'), (SELECT Value FROM Variables WHERE Name = 'myInsurer'));
INSERT INTO Variables(Name, Value) SELECT 'insurerFrontEnd', last_insert_rowid();

-- Project 2 for client 1
INSERT INTO projects (name, display_name, is_active, client_id, employer_id) VALUES ('InsurerBackEnd Modern Rewrite and Port', 'InsurerBackEnd', 1, (SELECT Value FROM Variables WHERE Name = 'myCompany'), (SELECT Value FROM Variables WHERE Name = 'myInsurer'));
INSERT INTO Variables(Name, Value) SELECT 'insurerBackEnd', last_insert_rowid();

INSERT INTO tasks (task_date, is_active) VALUES (date(now), 1);
INSERT INTO Variables(Name, Value) SELECT 'taskDate', last_insert_rowid();

INSERT INTO categories (name, description, is_active, project_id) VALUES ('Programming', 'The task of inputting code into the machine to achieve a business objective', (SELECT Value FROM Variables WHERE Name = 'insurerFrontEnd'));
INSERT INTO Variables(Name, Value) SELECT 'programming', last_insert_rowid();

INSERT INTO categories (name, description, is_active, project_id) VALUES ('Analysis', 'Analysing new batch of work and gathering research before starting coding', (SELECT Value FROM Variables WHERE Name = 'insurerFrontEnd'));
INSERT INTO Variables(Name, Value) SELECT 'analysis', last_insert_rowid();

INSERT INTO task_details(start_time, end_time, duration, description, is_active, project_id, task_id, category_id) VALUES('06:15:00', '07:00:00', '00:45:00', 'Conducting analysis of reusing Angular component for user form input and emailing back client', 1, (SELECT Value FROM Variables WHERE Name = 'insurerFrontEnd'), (SELECT Value FROM Variables WHERE Name = 'taskDate'), (SELECT Value FROM Variables WHERE Name = 'analysis'));
INSERT INTO task_details(start_time, end_time, duration, description, is_active, project_id, task_id, category_id) VALUES('07:15:00', '10:30:00', '03:45:00', 'Implemeting user form input and hooking up with back-end components and fixing misc issues along the way', 1, (SELECT Value FROM Variables WHERE Name = 'insurerFrontEnd'), (SELECT Value FROM Variables WHERE Name = 'taskDate'), (SELECT Value FROM Variables WHERE Name = 'programming'));
INSERT INTO task_details(start_time, end_time, duration, description, is_active, project_id, task_id, category_id) VALUES('10:31:00', '10:45:00', '00:15:00', 'Stand up - Update on previous Angular research and implementation of user input form', 1, (SELECT Value FROM Variables WHERE Name = 'insurerFrontEnd'), (SELECT Value FROM Variables WHERE Name = 'taskDate'), (SELECT Value FROM Variables WHERE Name = 'analysis'));
INSERT INTO task_details(start_time, end_time, duration, description, is_active, project_id, task_id, category_id) VALUES('10:46:00', '11:46:00', '01:00:00', 'Testing of component and adding unit tests', 1, (SELECT Value FROM Variables WHERE Name = 'insurerFrontEnd'), (SELECT Value FROM Variables WHERE Name = 'taskDate'), (SELECT Value FROM Variables WHERE Name = 'programming'));
INSERT INTO task_details(start_time, end_time, duration, description, is_active, project_id, task_id, category_id) VALUES('12:15:00', '15:05:00', '02:50:00', 'Implementing back-end component of user login input, adding validations, persisting to database and end-to-end testing', 1, (SELECT Value FROM Variables WHERE Name = 'insurerBackEnd'), (SELECT Value FROM Variables WHERE Name = 'taskDate'), (SELECT Value FROM Variables WHERE Name = 'programming'));

-- Other
INSERT INTO employers (name, is_active) VALUES ('PersonalCompany Ltd', 1);
