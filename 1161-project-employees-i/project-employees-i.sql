# Write your MySQL query statement below
# select E.experience , avg(E.experience) as average, P.project_id from Employee as E inner join on project as E where P.employee_id = E.employee_id group by(E.experience);

SELECT p.project_id,
       ROUND(AVG(e.experience_years), 2) AS average_years
FROM Project AS p
INNER JOIN Employee AS e
  ON p.employee_id = e.employee_id
GROUP BY p.project_id;