#pragma once

#include <ESPSupabase.h>
#include <Arduino.h>

// Initialize the internal Supabase instance
void supabaseBegin(const String& url, const String& key);

// Insert a user record (returns HTTP code)
int supabaseAddUser(const String& name);

// Reset the internal query builder state (call after operations)
void supabaseResetQuery();
