.. -*- coding: utf-8 -*-
.. _php:

PHP
---

.. module:: moro8

.. contents::
   :local:
      
Push Processor Status on Stack.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                push SR

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                - - - - - -

    .. container:: moro8-synopsis moro8-pre
                
                MODE          SYNTAX        HEX LEN TIM
                Implied       PHP           $08  1   3

The status register will be pushed with the break
flag and bit 5 set to 1.
