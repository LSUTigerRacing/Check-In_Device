#include "SupabaseWrapper.h"

static Supabase db;

void supabaseBegin(const String& url, const String& key) {
  db.begin(url, key);
}

int supabaseAddUser(const String& name) {
  String t = "[{\"name\": \"" + name + "\", \"created_at\": \"2025-11-07T01:43:47.178071+00:00\"}]";
  int code = db.insert("esp", t, false);
  return code;
}

void supabaseResetQuery() {
  db.urlQuery_reset();
}
