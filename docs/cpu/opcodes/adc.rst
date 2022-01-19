.. -*- coding: utf-8 -*-
.. _adc:

ADC
---

.. module:: moro8

.. contents::
   :local:
      
Add Memory to Accumulator with Carry.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                A + M + C -> A, C

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                + + + - - +

    .. container:: moro8-synopsis moro8-pre

        MODE          SYNTAX        HEX LEN TIM
        Immediate     ADC #$44      $69  2   2
        Zero Page     ADC $44       $65  2   3
        Zero Page,X   ADC $44,X     $75  2   4
        Absolute      ADC $4400     $6D  3   4
        Absolute,X    ADC $4400,X   $7D  3   4+
        Absolute,Y    ADC $4400,Y   $79  3   4+
        Indirect,X    ADC ($44,X)   $61  2   6
        Indirect,Y    ADC ($44),Y   $71  2   5+

