

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"
#include "cycle.h"


extern int semant_debug;
extern char *curr_filename;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol 
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
    arg         = idtable.add_string("arg");
    arg2        = idtable.add_string("arg2");
    Bool        = idtable.add_string("Bool");
    concat      = idtable.add_string("concat");
    cool_abort  = idtable.add_string("abort");
    copy        = idtable.add_string("copy");
    Int         = idtable.add_string("Int");
    in_int      = idtable.add_string("in_int");
    in_string   = idtable.add_string("in_string");
    IO          = idtable.add_string("IO");
    length      = idtable.add_string("length");
    Main        = idtable.add_string("Main");
    main_meth   = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any 
    //   user-defined class.
    No_class    = idtable.add_string("_no_class");
    No_type     = idtable.add_string("_no_type");
    Object      = idtable.add_string("Object");
    out_int     = idtable.add_string("out_int");
    out_string  = idtable.add_string("out_string");
    prim_slot   = idtable.add_string("_prim_slot");
    self        = idtable.add_string("self");
    SELF_TYPE   = idtable.add_string("SELF_TYPE");
    Str         = idtable.add_string("String");
    str_field   = idtable.add_string("_str_field");
    substr      = idtable.add_string("substr");
    type_name   = idtable.add_string("type_name");
    val         = idtable.add_string("_val");
}



ClassTable::ClassTable(Classes classes) : semant_errors(0) , error_stream(cerr) {
    classInfos = NULL;
    install_basic_classes();
    for(int i = classes->first(); classes->more(i); i = classes->next(i))
      install_one_class(classes->nth(i));
}

void ClassTable::install_basic_classes() {

    // The tree package uses these globals to annotate the classes built below.
   // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");
    
    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.
    
    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    // 
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class =
	class_(Object, 
	       No_class,
	       append_Features(
			       append_Features(
					       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
					       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
			       single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	       filename);

    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class = 
	class_(IO, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       single_Features(method(out_string, single_Formals(formal(arg, Str)),
										      SELF_TYPE, no_expr())),
							       single_Features(method(out_int, single_Formals(formal(arg, Int)),
										      SELF_TYPE, no_expr()))),
					       single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
			       single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
	       filename);  

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    Class_ Int_class =
	class_(Int, 
	       Object,
	       single_Features(attr(val, prim_slot, no_expr())),
	       filename);

    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
	class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //       
    Class_ Str_class =
	class_(Str, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       append_Features(
									       single_Features(attr(val, Int, no_expr())),
									       single_Features(attr(str_field, prim_slot, no_expr()))),
							       single_Features(method(length, nil_Formals(), Int, no_expr()))),
					       single_Features(method(concat, 
								      single_Formals(formal(arg, Str)),
								      Str, 
								      no_expr()))),
			       single_Features(method(substr, 
						      append_Formals(single_Formals(formal(arg, Int)), 
								     single_Formals(formal(arg2, Int))),
						      Str, 
						      no_expr()))),
	       filename);

  install_one_class(Object_class);
  install_one_class(IO_class);
  install_one_class(Int_class);
  install_one_class(Bool_class);
  install_one_class(Str_class);
}

void ClassTable::install_one_class(Class_ c) {
  ClassInfo* info = new ClassInfo();
  info->class_ = c;
  c->register_class_info(info);
  classInfos = new List<ClassInfo>(info, classInfos);
}

void class__class::register_class_info(ClassInfo* info) {
  info->name = name;
  info->parent = parent;
  for(int i = features->first(); features->more(i); i = features->next(i))
    features->nth(i)->register_class_info(info);
}

void method_class::register_class_info(ClassInfo* info) {
  MethodInfo *methodInfo = new MethodInfo();
  methodInfo->name = name;
  methodInfo->retType = return_type;
  info->methodInfos = new List<MethodInfo>(methodInfo, info->methodInfos);
  int formal_count = formals->len();
  for(int i = formals->first(); formals->more(i); i = formals->next(i))
    formals->nth(formal_count-1-i)->register_class_info(info); // ugly hack to ensure formal order
}

void attr_class::register_class_info(ClassInfo* info) {
  AttrInfo *attrInfo = new AttrInfo();
  attrInfo->name = name;
  attrInfo->type = type_decl;
  info->attrInfos = new List<AttrInfo>(attrInfo, info->attrInfos);
}

void formal_class::register_class_info(ClassInfo* info) {
  AttrInfo *attrInfo = new AttrInfo();
  attrInfo->name = name;
  attrInfo->type = type_decl;
  info->methodInfos->hd()->argInfos = new List<AttrInfo>(attrInfo, info->methodInfos->hd()->argInfos);
}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()                
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)  
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c)
{                                                             
    return semant_error(c->get_filename(),c);
}    

ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
{
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error()                  
{                                                 
    semant_errors++;                            
    return error_stream;
} 

void ClassTable::check_unique_var() {
    check_unique_class();
    check_unique_attr();
    check_unique_method();
    check_unique_formal();
}

void ClassTable::check_class_hierarchy() {
    check_class_parent_exist();
    check_class_acyclic();
}

void ClassTable::check_unique_class() {
  List<ClassInfo> *cl1, *cl2;
  ClassInfo *ci1, *ci2;
  for (cl1 = classInfos; cl1 != NULL; cl1 = cl1->tl()) {
    ci1 = cl1->hd();
    for (cl2 = classInfos; cl2 != cl1; cl2 = cl2->tl()) {
      ci2 = cl2->hd();
      if (ci1->name == ci2->name) {
        semant_error(ci2->class_);
      }
    }
  }
}

void ClassTable::check_unique_attr() {
  List<ClassInfo> *cl;
  ClassInfo *ci;
  List<AttrInfo> *al1, *al2;
  AttrInfo *ai1, *ai2;
  for (cl = classInfos; cl != NULL; cl = cl->tl()) {
    ci = cl->hd();
    for (al1 = ci->attrInfos; al1 != NULL; al1 = al1->tl()) {
      ai1 = al1->hd();
      for (al2 = ci->attrInfos; al2 != al1; al2 = al2->tl()) {
        ai2 = al2->hd();
        if (ai1->name == ai2->name) {
          semant_error(ci->class_);
        }
      }
    }
  }
}

void ClassTable::check_unique_method() {
  List<ClassInfo> *cl;
  ClassInfo *ci;
  List<MethodInfo> *ml1, *ml2;
  MethodInfo *mi1, *mi2;
  for (cl = classInfos; cl != NULL; cl = cl->tl()) {
    ci = cl->hd();
    for (ml1 = ci->methodInfos; ml1 != NULL; ml1 = ml1->tl()) {
      mi1 = ml1->hd();
      for (ml2 = ci->methodInfos; ml2 != ml1; ml2 = ml2->tl()) {
        mi2 = ml2->hd();
        if (mi1->name == mi2->name) {
          semant_error(ci->class_);
        }
      }
    }
  }
}

void ClassTable::check_unique_formal() {
  List<ClassInfo> *cl;
  ClassInfo *ci;
  List<MethodInfo> *ml;
  MethodInfo *mi;
  List<AttrInfo> *al1, *al2;
  AttrInfo *ai1, *ai2;
  for (cl = classInfos; cl != NULL; cl = cl->tl()) {
    ci = cl->hd();
    for (ml = ci->methodInfos; ml != NULL; ml = ml->tl()) {
      mi = ml->hd();
      for (al1 = mi->argInfos; al1 != NULL; al1 = al1->tl()) {
        ai1 = al1->hd();
        for (al2 = mi->argInfos; al2 != al1; al2 = al2->tl()) {
          ai2 = al2->hd();
          if (ai1->name == ai2->name) {
            semant_error(ci->class_);
          }
        }
      }
    }
  }
}

void ClassTable::check_class_parent_exist() {
  List<ClassInfo> *cl1, *cl2;
  ClassInfo *ci1, *ci2;
  bool found;
  for (cl1 = classInfos; cl1 != NULL; cl1 = cl1->tl()) {
    ci1 = cl1->hd();
    if (ci1->name == ci1->parent) { // forbid self inheritance
      semant_error(ci1->class_);
    }
    if (ci1->parent != No_class) { // must find parent
      found = false;
      for (cl2 = classInfos; cl2 != NULL; cl2 = cl2->tl()) {
        ci2 = cl2->hd();
        if (ci2->name == ci1->parent) {
          found = true;
          break;
        }
      }
      if (found == false) {
        semant_error(ci1->class_);
      }
    }
  }
}

void ClassTable::check_class_acyclic() {
  int vn = list_length(classInfos)+1;
  CycleDetector cd = CycleDetector(vn); // +1 for No_class
  List<ClassInfo> *cl1, *cl2;
  ClassInfo *ci1, *ci2;
  int index1, index2;
  index1 = 0;
  for (cl1 = classInfos; cl1 != NULL; cl1 = cl1->tl()) {
    index1++;
    ci1 = cl1->hd();
    index2 = 0;
    if (ci1->parent != No_class) {
      for (cl2 = classInfos; cl2 != NULL; cl2 = cl2->tl()) {
        index2++;
        ci2 = cl2->hd();
        if (ci2->name == ci1->parent) {
          break;
        }
      }
    }
    assert(index2 < vn);
    cd.addEdge(index2, index1);
  }
  if (cd.isCyclic()) {
    semant_error();
  }
}

/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant()
{
    initialize_constants();

    /* ClassTable constructor may do some semantic analysis */
    std::cout<<"1"<<std::endl;
    ClassTable *classtable = new ClassTable(classes);

    /* some semantic analysis code may go here */
    std::cout<<"2"<<std::endl;
    classtable->check_unique_var();
    std::cout<<"3"<<std::endl;
    classtable->check_class_hierarchy();
    std::cout<<"4"<<std::endl;
    if (classtable->errors()) {
      cerr << "Compilation halted due to static semantic errors." << endl;
      exit(1);
    }

    if (classtable->errors()) {
      cerr << "Compilation halted due to static semantic errors." << endl;
      exit(1);
    }
}

int curr_lineno = 1;
