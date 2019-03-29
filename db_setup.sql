-- wx-timesheets-tracker database setup scripts --

-- employers table
CREATE TABLE employers
(
    employer_id INTEGER PRIMARY KEY NOT NULL,
    name TEXT NOT NULL,
    date_created_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),
    date_modified_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),
    is_active INTEGER NOT NULL
);

-- projects table
CREATE TABLE projects
(
    project_id INTEGER PRIMARY KEY NOT NULL,
    name TEXT NOT NULL UNIQUE,
    date_created_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),
    date_modified_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),
    is_active INTEGER NOT NULL,
    
    employer_id INTEGER NOT NULL,
    
    FOREIGN KEY (employer_id) REFERENCES employers(employer_id)
);

-- tasks table
CREATE TABLE tasks
(
    task_id INTEGER PRIMARY KEY NOT NULL,
    task_date TEXT NOT NULL UNIQUE,
    date_created_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),
    date_modified_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),
    is_active INTEGER NOT NULL,
    
    project_id INTEGER NOT NULL,
    
    FOREIGN KEY (project_id) REFERENCES projects(project_id)
);

-- categories table
CREATE TABLE categories
(
    category_id INTEGER PRIMARY KEY NOT NULL,
    name TEXT NOT NULL,
    description TEXT NOT NULL,
    date_created_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),
    date_modified_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),
    is_active INTEGER NOT NULL,
    
    project_id INTEGER NOT NULL,
    
    FOREIGN KEY project_id REFERENCES projects(project_id)
);

-- task details table
CREATE TABLE task_details
(
    task_detail_id INTEGER PRIMARY KEY NOT NULL,
    start_time TEXT NOT NULL,
    end_time TEXT NOT NULL,
    description TEXT NOT NULL,
    date_created_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),
    date_modified_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),
    is_active INTEGER NOT NULL,
    
    project_id INTEGER NOT NULL,
    task_id INTEGER NOT NULL,
    category_id INTEGER NOT NULL,
    
    FOREIGN KEY project_id REFERENCES projects(project_id),
    FOREIGN KEY task_id REFERENCES tasks(task_id),
    FOREIGN KEY category_id REFERENCES categories(category_id)
);