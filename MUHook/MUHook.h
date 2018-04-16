//
//  MUHook.h
//
//  Created by Shuang Wu on 2017/2/9.
//
//  Copyright Shuang Wu 2017
//

#import <Foundation/Foundation.h>
#import <objc/runtime.h>
#import <objc/message.h>
#import "fishhook.h"
#import "MUHRuntime.h"
#import "NSObject+MUHook.h"

/**
 *  Hook Usage
 *
 *  In .h file:
 *
 *  void MUHInitClass(MUTableViewController);
 *
 *  In .m file:
 *
 *  MUHHookImplementation(MUTableViewController, cellForRowAtIndexPath, UITableViewCell *, UITableView *tableView, NSIndexPath *indexPath) {
 *      UITableViewCell *cell = MUHOrig(MUTableViewController, cellForRowAtIndexPath, UITableView *tableView, NSIndexPath *indexPath);
 *      // some code to modify cell
 *      return cell;
 *  }
 *
 *  void MUHInitClass(MUTableViewController) {
 *      MUHookMessageEx(MUTableViewController, @selector(tableView:cellForRowAtIndexPath:), cellForRowAtIndexPath);
 *  }
 *
 *  In main():
 *
 *  MUHInitClass(MUTableViewController);
 *
 **/


/**
 *  Create Usage
 *
 *  In .h file:
 *
 *  void MUHInitClass(MUTableViewController);
 *
 *  In .m file:
 *
 *  MUHHookImplementation(MUTableViewController, cellForRowAtIndexPath, UITableViewCell *, UITableView *tableView, NSIndexPath *indexPath) {
 *      UITableViewCell *cell = MUHOrig(MUTableViewController, cellForRowAtIndexPath, UITableView *tableView, NSIndexPath *indexPath);
 *      // some code to modify cell
 *      return cell;
 *  }
 *
 *  void MUHInitClass(MUTableViewController) {
 *      MUHAllocateClass(UIViewController, MUTableViewController, 0);
 *      MUHAddMessage(MUTableViewController, @selector(tableView:cellForRowAtIndexPath:), cellForRowAtIndexPath, @@:@@);
 *      ...
 *      MUHRegisterClass(MUTableViewController);
 *  }
 *
 *  In main():
 *
 *  MUHInitClass(MUTableViewController);
 *
 **/

#pragma mark - Main

#define MUHMain                                 static __attribute__((constructor)) initialize

#pragma mark - Quick Statement

#define MUHClass(c)                             objc_getClass(#c)

#define MUHSendClassMsg(c, factory)             [MUHClass(c) factory]

#define MUHAlloc(c)                             ((c *)[MUHClass(c) alloc])

#define MUHAllocInitWith(c, init)               [MUHAlloc(c) init]

#define MUHGetObjectAsct(obj, name)             MUGetAsctValue(obj, #name)
#define MUHSetObjectAsct(obj, name, value, mm)  MUSetAsctValue(obj, #name, value, MUHAssosiationType_##mm)

#define MUHGetSelfAsct(name)                    MUGetAsctValue(self, #name)
#define MUHSetSelfAsct(name, value, mm)         MUSetAsctValue(self, #name, value, MUHAssosiationType_##mm)

#define MUHGetIvar(obj, ivar)                   MUGetInstanceIvar(obj, #ivar)
#define MUHSetIvar(obj, ivar, value)            MUSetInstanceIvar(obj, #ivar, value)

#define MUHGetSelfIvar(ivar)                    MUGetInstanceIvar(self, #ivar)
#define MUHSetSelfIvar(ivar, value)             MUSetInstanceIvar(self, #ivar, value)

#define MUHInitClass(c)                         init_##c ()

#pragma mark - Implementation

#define MUHInstanceImplementation(c, name, returnType, args...) \
static returnType (*_unique_objc_ori$##c##$##name)   ( c * obj, SEL, ##args );\
static returnType   _unique_objc_new$##c##$##name    ( c * self, SEL _cmd, ##args )

#define MUHClassImplementation(c, name, returnType, args...) \
static returnType (*_unique_objc_ori$##c##$##name)   ( Class self, SEL _cmd, ##args );\
static returnType   _unique_objc_new$##c##$##name    ( Class self, SEL _cmd, ##args )

#define MUHSymbolImplementation(symbol, returnType, args...) \
static returnType (*_unique_symbol_ori$##symbol)(args);\
static returnType   _unique_symbol_new$##symbol (args)

#pragma mark - Execute Orig or Super

#define MUHOrig(c, name, args...)               (!_unique_objc_ori$##c##$##name ? 0 : _unique_objc_ori$##c##$##name (self, _cmd, ##args))
#define MUHSuper(c, name, args...)              (!_unique_objc_ori$##c##$##name ? 0 : _unique_objc_ori$##c##$##name (self, _cmd, ##args))

#define MUHSymbolOrig(symbol, args...)          _unique_symbol_ori$##symbol(args)

#pragma mark - Hook

#define MUHHookInstanceMessage(c, name, sel) \
MUHookInstanceMessageEx(objc_getClass( #c ), @selector(sel), (IMP)&_unique_objc_new$##c##$##name, (IMP*)&_unique_objc_ori$##c##$##name);

#define MUHHookClassMessage(c, name, sel) \
MUHookClassMessageEx(objc_getClass( #c ), @selector(sel), (IMP)&_unique_objc_new$##c##$##name, (IMP*)&_unique_objc_ori$##c##$##name);

#define MUHHookSymbolFunction(symbol) rebind_symbols((struct rebinding[1]){{#symbol, _unique_symbol_new$##symbol, (void *)&_unique_symbol_ori$##symbol}}, 1)

#pragma mark - Create

#define MUHCreateClass(c, sc) MUCreateClass(#c, #sc)

#define MUHAddInstanceMethod(c, name, sel, encode) MUAddInstanceMessageEx(objc_getClass( #c ), @selector(sel), (IMP)&_unique_objc_new$##c##$##name, #encode , (IMP*)&_unique_objc_ori$##c##$##name)

#define MUHAddClassMethod(c, name, sel, encode) MUAddClassMessageEx(objc_getMetaClass( #c ), @selector(sel), (IMP)&_unique_objc_new$##c##$##name, #encode , (IMP*)&_unique_objc_ori$##c##$##name)

#pragma mark - Function

Class MUCreateClass(const char *className, const char *superClass);

void MUHookInstanceMessageEx(Class _class, SEL sel, IMP imp, IMP *result);

void MUHookClassMessageEx(Class _class, SEL sel, IMP imp, IMP *result);

void MUAddInstanceMessageEx(Class _class, SEL sel, IMP imp, const char *typeEncoding, IMP *result);

void MUAddClassMessageEx(Class _class, SEL sel, IMP imp, const char *typeEncoding, IMP *result);

id MUGetAsctValue(id obj, const char *name);
void MUSetAsctValue(id obj, const char *name, id value, MUHAssosiationType memory);

id MUGetInstanceIvar(id obj, const char *ivar);
void MUSetInstanceIvar(id obj, const char *ivar, id value);

Class MUAllocateClassPair(Class superClass, const char *className, size_t extraBytes);

void MURegisterClassPair(Class _class);