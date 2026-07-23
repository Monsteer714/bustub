select 
	p.nameFirst || ' (' || p.nameGiven || ') ' || p.nameLast as name, max(a.HR) as max_HR
from people as p 
join appearances as a 
	on p.playerID = a.playerID
join collegeplaying as cp
	on p.playerID = cp.playerID
join schools as s
	on cp.schoolID = s.schoolID
where s.state = 'PA'
group by 
	p.nameFirst,
	p.nameGiven, 
	p.nameLast
order by 
	max_HR desc,
	p.nameFirst
limit 10
;			
