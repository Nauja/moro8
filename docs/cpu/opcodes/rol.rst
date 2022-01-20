.. -*- coding: utf-8 -*-
.. _rol:

ROL
---

.. module:: moro8

.. contents::
   :local:
      
Rotate One Bit Left (Memory or Accumulator).

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                C <- [76543210] <- C

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                + + + - - -

    .. container:: moro8-synopsis moro8-pre

                MODE          SYNTAX        HEX LEN TIM
                Accumulator   ROL A         $2A  1   2
                Zero Page     ROL $44       $26  2   5
                Zero Page,X   ROL $44,X     $36  2   6
                Absolute      ROL $4400     $2E  3   6
                Absolute,X    ROL $4400,X   $3E  3   7

ROL shifts all bits left one position. The Carry is shifted into bit 0 and the original bit 7 is shifted into the Carry.