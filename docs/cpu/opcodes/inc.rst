.. -*- coding: utf-8 -*-
.. _inc:

INC
---

.. module:: moro8

.. contents::
   :local:

Increment Memory by One.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                M + 1 -> M

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                + + - - - -

    .. container:: moro8-synopsis moro8-pre
                
                MODE          SYNTAX        HEX LEN TIM
                Zero Page     INC $44       $E6  2   5
                Zero Page,X   INC $44,X     $F6  2   6
                Absolute      INC $4400     $EE  3   6
                Absolute,X    INC $4400,X   $FE  3   7
