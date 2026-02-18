#pragma once

#include <ESPSupabase.h>
#include <Arduino.h>

// Initialize the internal Supabase instance
void supabaseBegin(const String& url, const String& key);

// Insert a user record (returns HTTP code)
void AddUserSupabase(const String user);

// Insert a user record in Office (returns HTTP code)
void AddInOffice(String user);

// Insert a user record in Shop (returns HTTP code)
void AddInShop(String user);

// Remove a user record from Office(returns HTTP code)
void RemoveInOffice(String user);

// Remove a user record from Shop(returns HTTP code)
void RemoveInShop(String user);

// Reset the internal query builder state (call after operations)
void supabaseResetQuery();
