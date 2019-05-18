SELECT task_details.task_detail_id, tasks.task_date,
task_details.start_time,
task_details.end_time,
task_details.duration,
task_details.description as description,
categories.name as category_name,
projects.display_name as project_name
FROM task_details
INNER JOIN tasks ON task_details.task_id = tasks.task_id
INNER JOIN categories ON task_details.category_id = categories.category_id
INNER JOIN projects ON task_details.project_id = projects.project_id
WHERE task_date = '2019-04-23'

SELECT projects.project_id,
projects.name AS project_name,
projects.display_name,
projects.date_created_utc,
projects.date_modified_utc,
projects.is_active,
projects.employer_id,
projects.client_id,
employers.name AS employer_name,
clients.name AS client_name
FROM projects
INNER JOIN employers ON projects.employer_id = employers.employer_id
INNER JOIN clients ON projects.client_id = clients.client_id
WHERE projects.project_id = 0

SELECT categories.category_id,
categories.name AS category_name,
categories.description,
categories.date_created_utc,
categories.date_modified_utc,
categories.is_active,
categories.project_id,
projects.name AS project_name
FROM categories
INNER JOIN projects ON categories.project_id = projects.project_id
WHERE categories.category_id = 0
