MIRANDA (ground + objects)
-------------------------

[![Build Status](https://travis-ci.org/nardinan/miranda.svg?branch=master)](https://travis-ci.org/nardinan/miranda)

Note: Miranda is a 160 characters-per-row library!

#Introduction
Miranda is an experimental C library that I started in 2014 and that is still under development.
The idea behind Miranda is to create a dynamic object-oriented interface for the C programming language that allows users to easily declare their own classes, define private and public methods, inherit, override and much, much more.

## Objects
In order to correctly understand how Miranda works, we should analyze first how the library handles objects.
Each object is defined by a structure that contains some information (e.g. its *type* and where the object has been instantiated), some private fields (e.g. caches) and a list of **virtual tables** and **attributes**.
If a class doesn't inherit from any other class (like *memory* or *mutex* classes), the two lists (virtual tables and attributes) will contain a single entry: methods and attributes of the defined class.
As soon as you start to inherit from other classes, the virtual tables and the attributes lists of the inherited classes are pushed to these two lists.
In this example you can understand how a class that defines a character entity in a game is handled by Miranda.

[![example_class.png](https://s15.postimg.org/kp5d50trv/example_class.png)](https://postimg.org/image/jzmksnt87/)

*character* class inherits from *memory*, *mutex* and *entity* classes, while *entity* class inherits from *drawable* class (In Miranda, a class that doesn't inherit from *memory* class is considered **abstract**).

### Define classes
To define a class you should first declare the **attributes structure** in the header. For instance the class *point* which contains a x and y coordinate (double), is defined in this way (objects/geometry/point.obj.h):
```c
d_declare_class(point) {
    struct s_attributes head; // this is mandatory for Miranda
    double x, y;
} d_declare_class_tail(point);
```
As you can see from the example above, the first entry in the structure **has to be** a *s\_attributes* instance. This is required in order to help Miranda collect all the entries instances in a single list by casting a generic attributes structure to a *s\_attributes* structure.
Now is time to declare a function that can be used as an object-specific *new* function. For the *point* class we have:
```c
extern struct s_object *f_point_new(struct s_object *self, double x, double y); 
```
*f\_point\_new* (but, in general, each "new" function present in Miranda) is a function that takes an empty instance of a generic object (already marked as a *point* object through the *d\_new* function) and fills the class attributes structure with the correct information.
```c
struct s_point_attributes *p_point_alloc(struct s_object *self) {
    /* the following function (d_prepare) initializes an empty object (self) to a specific type (point)
     * by pushing inside a new instance of point's attributes structure and a reference to the methods 
     * table.
     * The function returns then the reference to the point's attributes structure allocated.
     */
    struct s_point_attributes *result = d_prepare(self, point);
    
    /* here we can inherit from other classes by passing our object to the other 'new' functions
     */
    f_memory_new(self); // the object now inherits from memory. Is not abstract anymore.
    f_mutex_new(self);  // the object now inherits from mutex
    return result;
}

struct s_object *f_point_new(struct s_object *self, double x, double y) {
    /* each object has is own attributes type structure s_<object type>_attributes
     */
    struct s_point_attributes *attributes = p_point_alloc(self);
    attributes->x = x;
    attributes->y = y;
    return self;
}
```
As soon as you want to declare methods, you need to use the *d\_declare\_method* function specifying the object type (*point* in this specific case) and the name of the method. Remember to use short names because, at the very end, the real name of the method is translated by Miranda to m_\[class name\]_\[method\] (e.g. m\_point\_set\_x).
For each method, the first parameters, **has to be** a pointer to the object and this is the only way to reach the **attributes structure(s)**.
```c
d_declare_method(point, set_point)(struct s_object *self, struct s_object *source); 
d_declare_method(point, set_x)(struct s_object *self, double x); 
d_declare_method(point, set_y)(struct s_object *self, double y); 
d_declare_method(point, get)(struct s_object *self, double *x, double *y); 
d_declare_method(point, add)(struct s_object *self, double x, double y); 
d_declare_method(point, subtract)(struct s_object *self, double x, double y); 
d_declare_method(point, distance)(struct s_object *self, struct s_object *other, double *distance, double *distance_square);
```
In this case, the class *point* doesn't allocate any resource so we don't need to declare or define a *delete* method.
Once you have defined each method by using the *d\_define\_method* function, you need to define the final class too by collecting all these methods into a single "virtual table" through the function *d_define_class*.
```c
d_define_method(point, set_point)(struct s_object *self, struct s_object *source) {
    /* do something */
    return self;
}

d_define_method(point, set_x)(struct s_object *self, double x) {
    /* do something */
    return self;
}

d_define_method(point, set_y)(struct s_object *self, double y) {
    /* do something */
    return self;
}

d_define_method(point, get)(struct s_object *self, double *x, double *y) {
    /* do something */
    return self;
}

d_define_method(point, add)(struct s_object *self, double x, double y) {
    /* do something */
    return self;
}

d_define_method(point, subtract)(struct s_object *self, double x, double y) {
    /* do something */
    return self;
}

d_define_method(point, distance)(struct s_object *self, struct s_object *other, double *distance, double *distance_square) {
    /* do something */
    return self;
}

d_define_class(point) {
    d_hook_method(point, e_flag_public, set_point),
    d_hook_method(point, e_flag_private, set_x),
    d_hook_method(point, e_flag_private, set_y),
    d_hook_method(point, e_flag_public, get),
    d_hook_method(point, e_flag_public, add),
    d_hook_method(point, e_flag_public, subtract),
    d_hook_method(point, e_flag_public, distance),
    d_hook_method_tail
};
```
By using the *d\_hook\_method* function Miranda allows to mark a method as public or private (private methods cannot be recalled from other .c files).
For each method, the access to the **attributes structure** is obtained thanks to the function *d\_cast* which takes a generic object and the type of attributes that you want to retrieve (e.g. *point* attributes).
```c
struct s_point_attributes *this = d_cast(self, point);
```
If for some reason an object inherits from other methods (e.g. *memory* or *mutex*), the following code is also allowed:
```c
struct s_point_attributes *this = d_cast(self, point);
struct s_memory_attributes *memory_this = d_cast(self, memory);
struct s_mutex_attributes *mutex_this = d_cast(self, mutex);
```
### Overriding methods
under development
### Work with Miranda
Once your class is ready to be used (i.e. *point* class) you probably want to create an object. To achieve this goal you need to use the object-specific *new* function that we have declared and defined above:
```c
     struct s_object *point_obj = f_point_new(d_new(point), 12.5, 12.5);
```
Et voila! The function *d\_new* creates a new empty object instance marked with the same type specified as parameter (*point* in this specific example) and the *f\_point\_new* function does the rest by initializing all the attributes.
Through the function *d\_call* now, is possible to call all the methods of our object following an easy-peasy syntax. The function takes as parameter the object itself, the symbol of the method that we want to use (m_\[class name\]_\[method\]), and the parameters of the method itself.
```c
    /*
     * d_declare_method(point, add)(struct s_object *self, double x, double y);
     */
     d_call(point_obj, m_point_add, 45.5, 45.0);
```
A method which is an override of an original method from another class, can be called by using the same *d\_call* function that we saw above. Obviously this calls the latest implementation presents in the *virtual table* list.
Let's pretend, for argument's sake, that the method *retain* of *point* overrides the same-named method of *memory*.
By using:
```c
     d_call(point_obj, m_memory_retain, <parameters>);
```
You are referring to the *retain* function presents in the object class. To access the original function, you should use *d\_call\_owner* function:
```c
     d_call_owner(point_obj, memory, m_memory_retain, <parameters>);
```
Easy, uh?
# Conclusions
Miranda is an interesting project and, right now, it is used as game engine for a graphical adventure that I am writing: *Pomodoro: bad things in Perugia*. This adventure is an interesting opportunity to test, verify and improve all the Miranda's modules.
At the current state there is only a single test file in the "test" directory (which, after the customary installation procedure 'make + make install' of Miranda, can be compiled with `gcc -lmiranda_ground -lmiranda_objects test.one.c -o test.one`)
