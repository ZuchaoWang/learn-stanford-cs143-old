(*
 *  Programming Assignment 1
 *    Implementation of a simple stack machine.
 *)

 Class List inherits IO { 
	isNil() : Bool { { abort(); true; } };

	cons(hd : String) : Cons {
	  (let new_cell : Cons <- new Cons in
		new_cell.init(hd,self)
	  )
	};

	car() : String { { abort(); new String; } };

	cdr() : List { { abort(); new List; } };
	
	print_list() : Object { abort() };
};

Class Cons inherits List {
	xcar : String;
	xcdr : List;

	isNil() : Bool { false };

	init(hd : String, tl : List) : Cons {
	  {
	    xcar <- hd;
	    xcdr <- tl;
	    self;
	  }
	};
	  
	car() : String { xcar };

	cdr() : List { xcdr };

	print_list() : Object {
		{
		     out_string(xcar);
		     out_string("\n");
		     xcdr.print_list();
		}
	};
};

Class Nil inherits List {
	isNil() : Bool { true };

	print_list() : Object { true };
};

Class Stack {
  l: List <- new Nil;

  push(x: String) : Stack {
    {
      l <- l.cons(x);
      self;
    }
  };

  peek() : String {
    l.car()
  };

  pop() : String {
    (let x : String in {
      x <- l.car();
      l <- l.cdr();
      x;
    })
  };

  empty() : Bool {
    l.isNil()
  };
};

class Main inherits IO {
  s : Stack <- new Stack;

  main() : Object {
    {
      s.push("1").push("2").push("3").pop();
      while (not s.empty()) 
        loop {
          out_string(s.pop());
          out_string("\n");
        } pool;
    }
  };
};
