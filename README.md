MIRANDA (ground + objects)
-------------------------

[![Build Status](https://travis-ci.org/nardinan/miranda.svg?branch=master)](https://travis-ci.org/nardinan/miranda)

Note: Miranda is a 160 characters-per-row library!

Miranda is an experimental C library that I started in 2014 and that is still under development. The idea behind Miranda is to create an efficient yet dynamic object-oriented interface for C that allows users to declare easily their own classes, define private and public methods, inherit, override and much, much more.

Now Miranda is used as a Linux/Mac 2D game engine and River (Miranda's WYSIWYG) is under development.

At the current state there is only a single test file in the "test" directory (which, after the classical procedure 'make + make install' of Miranda, can be compiled with `gcc -lmiranda_ground -lmiranda_objects test.one.c -o test.one`)
