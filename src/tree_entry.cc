/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/tree_entry.h"
#include "../include/oid.h"
#include "../include/repo.h"
#include "../include/object.h"

#include "../include/functions/utilities.h"
#include "../include/functions/string.h"

using namespace v8;
using namespace node;

GitTreeEntry::GitTreeEntry(git_tree_entry *raw) {
  this->raw = raw;
}

GitTreeEntry::~GitTreeEntry() {
  git_tree_entry_free(this->raw);
}

void GitTreeEntry::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("TreeEntry"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "name", Name);
  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "type", Type);
  NODE_SET_PROTOTYPE_METHOD(tpl, "filemode", filemode);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getObject", GetObject);


  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("TreeEntry"), constructor_template);
}

Handle<Value> GitTreeEntry::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_tree_entry is required.")));
  }

  GitTreeEntry* object = new GitTreeEntry((git_tree_entry *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitTreeEntry::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitTreeEntry::constructor_template->NewInstance(1, argv));
}

git_tree_entry *GitTreeEntry::GetValue() {
  return this->raw;
}


Handle<Value> GitTreeEntry::Name(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_tree_entry_name(
    ObjectWrap::Unwrap<GitTreeEntry>(args.This())->GetValue()
  );


  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

Handle<Value> GitTreeEntry::Oid(const Arguments& args) {
  HandleScope scope;
  

  const git_oid * result = git_tree_entry_id(
    ObjectWrap::Unwrap<GitTreeEntry>(args.This())->GetValue()
  );


  Handle<Value> to;
    to = GitOid::New((void *)result);
  return scope.Close(to);
}

Handle<Value> GitTreeEntry::Type(const Arguments& args) {
  HandleScope scope;
  

  git_otype result = git_tree_entry_type(
    ObjectWrap::Unwrap<GitTreeEntry>(args.This())->GetValue()
  );


  Handle<Value> to;
    to = Number::New(result);
  return scope.Close(to);
}

Handle<Value> GitTreeEntry::filemode(const Arguments& args) {
  HandleScope scope;
  

  git_filemode_t result = git_tree_entry_filemode(
    ObjectWrap::Unwrap<GitTreeEntry>(args.This())->GetValue()
  );


  Handle<Value> to;
    to = Number::New(result);
  return scope.Close(to);
}

Handle<Value> GitTreeEntry::GetObject(const Arguments& args) {
  HandleScope scope;
  
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  GetObjectBaton* baton = new GetObjectBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args[0]);
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->entryReference = Persistent<Value>::New(args.This());
  baton->entry = ObjectWrap::Unwrap<GitTreeEntry>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, GetObjectWork, (uv_after_work_cb)GetObjectAfterWork);

  return Undefined();
}

void GitTreeEntry::GetObjectWork(uv_work_t *req) {
  GetObjectBaton *baton = static_cast<GetObjectBaton *>(req->data);
  int result = git_tree_entry_to_object(
    &baton->object_out, 
    baton->repo, 
    baton->entry
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitTreeEntry::GetObjectAfterWork(uv_work_t *req) {
  HandleScope scope;
  GetObjectBaton *baton = static_cast<GetObjectBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = GitObject::New((void *)baton->object_out);
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      result
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  } else if (baton->error) {
    Handle<Value> argv[1] = {
      GitError::WrapError(baton->error)
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
  } else {
    baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
  }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->repoReference.Dispose();
  baton->entryReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

Persistent<Function> GitTreeEntry::constructor_template;
