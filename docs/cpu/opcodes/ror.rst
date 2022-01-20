.. -*- coding: utf-8 -*-
.. _ror:

ROR
---

.. module:: moro8

.. contents::
   :local:
      
Rotate One Bit Right (Memory or Accumulator).

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                C -> [76543210] -> C

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                + + + - - -

    .. container:: moro8-synopsis moro8-pre

                MODE          SYNTAX        HEX LEN TIM
                Accumulator   ROR A         $6A  1   2
                Zero Page     ROR $44       $66  2   5
                Zero Page,X   ROR $44,X     $76  2   6
                Absolute      ROR $4400     $6E  3   6
                Absolute,X    ROR $4400,X   $7E  3   7

ROR shifts all bits right one position. The Carry is shifted into bit 7 and the original bit 0 is shifted into the Carry.