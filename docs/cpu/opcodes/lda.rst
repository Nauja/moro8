.. -*- coding: utf-8 -*-
.. _lda:

LDA
---

.. module:: moro8

.. contents::
   :local:
      
Load Accumulator with Memory.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                M -> A

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                + + - - - -

    .. container:: moro8-synopsis moro8-pre

                MODE          SYNTAX        HEX LEN TIM
                Immediate     LDA #$44      $A9  2   2
                Zero Page     LDA $44       $A5  2   3
                Zero Page,X   LDA $44,X     $B5  2   4
                Absolute      LDA $4400     $AD  3   4
                Absolute,X    LDA $4400,X   $BD  3   4+
                Absolute,Y    LDA $4400,Y   $B9  3   4+
                Indirect,X    LDA ($44,X)   $A1  2   6
                Indirect,Y    LDA ($44),Y   $B1  2   5+
