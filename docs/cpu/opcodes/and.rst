.. -*- coding: utf-8 -*-
.. _and:

AND
---

.. module:: moro8

.. contents::
   :local:
      
AND Memory with Accumulator.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                A AND M -> A

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                + + - - - -

    .. container:: moro8-synopsis moro8-pre

        MODE          SYNTAX        HEX LEN TIM
        Immediate     AND #$44      $29  2   2
        Zero Page     AND $44       $25  2   3
        Zero Page,X   AND $44,X     $35  2   4
        Absolute      AND $4400     $2D  3   4
        Absolute,X    AND $4400,X   $3D  3   4+
        Absolute,Y    AND $4400,Y   $39  3   4+
        Indirect,X    AND ($44,X)   $21  2   6
        Indirect,Y    AND ($44),Y   $31  2   5+
