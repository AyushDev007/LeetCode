# Write your MySQL query statement below
select e.name , Eu.unique_id from employees as e left join EmployeeUNI as Eu on e.id=Eu.id;