-- Simple create function
function create ()
   create_pre()
   create_post()

   -- Inherit some properties from the parent (leader)
   local p = ai.pilot()
   local l = p:leader()
   if l then
      local lmem = l:memory()
      mem.atk_kill = lmem.atk_kill
      mem.enemyclose = lmem.enemyclose
      if not mem.mothership then
         mem.aggressive = lmem.aggressive
      end
   end
end

-- They don't investigate
function should_investigate ()
   return false
end

-- Just tries to guard mem.escort
function idle ()
   ai.pushtask("follow_fleet")
end
