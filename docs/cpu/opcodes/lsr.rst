.. -*- coding: utf-8 -*-
.. _lsr:

LSR
---

.. module:: moro8

.. contents::
   :local:
      
Shift One Bit Right (Memory or Accumulator).

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                0 -> [76543210] -> C

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                0 + + - - -

    .. container:: moro8-synopsis moro8-pre

                MODE          SYNTAX        HEX LEN TIM
                Accumulator   LSR A         $4A  1   2
                Zero Page     LSR $44       $46  2   5
                Zero Page,X   LSR $44,X     $56  2   6
                Absolute      LSR $4400     $4E  3   6
                Absolute,X    LSR $4400,X   $5E  3   7

LSR shifts all bits right one position. 0 is shifted into bit 7 and the original bit 0 is shifted into the Carry.