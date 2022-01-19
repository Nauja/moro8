.. -*- coding: utf-8 -*-
.. _tsx:

TSX
---

.. module:: moro8

.. contents::
   :local:
      
Transfer Stack Pointer to Index X.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                SP -> X

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                + + - - - -

    .. container:: moro8-synopsis moro8-pre
                
                MODE          SYNTAX        HEX LEN TIM
                Implied       TSX           $BA  1   2
