#include "SupabaseWrapper.h"

static Supabase db;

void supabaseBegin(const String& url, const String& key) {
  db.begin(url, key);
}

void AddUserSupabase(String user){
  String t = "[{\"name\": \"" + user + "\"}]";
  int code  = db.insert("esp",t,true); //(String: table name, String: JSON, Bool: Upsert )
  Serial.println(code);
}

void AddInOffice(String user){
  String t = "[{\"name\": \"" + user + "\", \"in_office\": true}]";
  int code  = db.insert("esp",t,true); //(String: table name, String: JSON, Bool: Upsert )
  Serial.println(code);
}

void AddInShop(String user){
  String t = "[{\"name\": \"" + user + "\", \"in_shop\": true}]";
  int code  = db.insert("esp",t,true); //(String: table name, String: JSON, Bool: Upsert )
  Serial.println(code);
}

void RemoveInOffice(String user){
  String t = "[{\"name\": \"" + user + "\", \"in_office\": false}]";
  int code  = db.insert("esp",t,true); //(String: table name, String: JSON, Bool: Upsert )
  Serial.println(code);
}

void RemoveInShop(String user){
  String t = "[{\"name\": \"" + user + "\", \"in_shop\": false}]";
  int code  = db.insert("esp",t,true); //(String: table name, String: JSON, Bool: Upsert )
  Serial.println(code);
}

void supabaseResetQuery() {
  db.urlQuery_reset();
}
