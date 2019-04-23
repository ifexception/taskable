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
