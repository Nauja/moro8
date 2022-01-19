.. -*- coding: utf-8 -*-
.. _asl:

ASL
---

.. module:: moro8

.. contents::
   :local:
      
Shift Left One Bit (Memory or Accumulator).

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                C <- [76543210] <- 0

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                + + + - - -

    .. container:: moro8-synopsis moro8-pre

        MODE          SYNTAX        HEX LEN TIM
        Accumulator   ASL A         $0A  1   2
        Zero Page     ASL $44       $06  2   5
        Zero Page,X   ASL $44,X     $16  2   6
        Absolute      ASL $4400     $0E  3   6
        Absolute,X    ASL $4400,X   $1E  3   7

ASL shifts all bits left one position. 0 is shifted into bit 0 and the original bit 7 is shifted into the Carry.