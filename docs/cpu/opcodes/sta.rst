.. -*- coding: utf-8 -*-
.. _sta:

STA
---

.. module:: moro8

.. contents::
   :local:
      
Store Accumulator in Memory.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                A -> M

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                - - - - - -

    .. container:: moro8-synopsis moro8-pre

                MODE          SYNTAX        HEX LEN TIM
                Zero Page     STA $44       $85  2   3
                Zero Page,X   STA $44,X     $95  2   4
                Absolute      STA $4400     $8D  3   4
                Absolute,X    STA $4400,X   $9D  3   5
                Absolute,Y    STA $4400,Y   $99  3   5
                Indirect,X    STA ($44,X)   $81  2   6
                Indirect,Y    STA ($44),Y   $91  2   6
