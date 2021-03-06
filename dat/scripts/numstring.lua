-- Converts an integer into a human readable string, delimiting every third digit with a comma.
-- Note: rounds input to the nearest integer. Primary use is for payment descriptions.
function numstring(number)
   number = math.floor(number + 0.5)
   local numberstring = ""
   while number >= 1000 do
      numberstring = string.format( ",%03d%s", number % 1000, numberstring )
      number = math.floor(number / 1000)
   end
   numberstring = number % 1000 .. numberstring
   return numberstring
end


--[[
-- @brief Properly converts a number of credits to a string.
--
-- Should be used everywhere a number of credits is displayed.
--
-- @usage tk.msg("", _("You have been paid %s."):format(creditstring(credits)))
--
--    @param credits Number of credits.
--    @return A string taking the form of "X ¤".
--]]
function creditstring( credits )
   return gettext.ngettext("%s ¤", "%s ¤", credits):format(
         numstring(credits) )
end


--[[
-- @brief Properly converts a number of tonnes to a string, utilizing ngettext.
--
-- This adds "tonnes" to the output of numstring in a translatable way.
-- Should be used everywhere a number of tonnes is displayed.
--
-- @usage tk.msg("", _("You are carrying %s."):format(tonnestring(tonnes)))
--
--    @param tonnes Number of tonnes.
--    @return A string taking the form of "X tonne" or "X tonnes".
--]]
function tonnestring( tonnes )
   return gettext.ngettext("%s tonne", "%s tonnes", tonnes):format(
         numstring(tonnes) )
end


--[[
-- @brief Like tonnestring, but for abbreviations.
--
--    @param tonnes Number of tonnes.
--    @return A short string like "22 t" describing the given mass.
--]]
function tonnestring_short( tonnes )
   -- Translator note: this form represents an abbreviation of "_ tonnes".
   return gettext.ngettext( "%d t", "%d t", tonnes ):format( tonnes )
end


--[[
-- @brief Properly converts a number of jumps to a string, utilizing ngettext.
--
-- This adds "jumps" to the output of numstring in a translatable way.
-- Should be used everywhere a number of jumps is displayed.
--
-- @usage tk.msg("", _("The system is %s away."):format(jumpstring(jumps)))
--
--    @param jumps Number of jumps.
--    @return A string taking the form of "X jump" or "X jumps".
--]]
function jumpstring( jumps )
   return gettext.ngettext("%s jump", "%s jumps", jumps):format(
         numstring(jumps) )
end


--[[
-- @brief Properly converts a number of times (occurrences) to a string,
-- utilizing ngettext.
--
-- This adds "times" to the output of numstring in a translatable way.
-- Should be used everywhere a number of occurrences is displayed.
--
-- @usage tk.msg("", _("Brush your teeth % per day."):format(timestring(times)))
--
--    @param times Number of times.
--    @return A string taking the form of "X time" or "X times".
--]]
function timestring( times )
   return gettext.ngettext("%s time", "%s times", times):format(
         numstring(times) )
end
