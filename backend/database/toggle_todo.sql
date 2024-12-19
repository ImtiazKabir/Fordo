UPDATE todos
SET is_done = CASE 
                WHEN is_done = 0 THEN 1 
                ELSE 0 
              END
WHERE id = ?
