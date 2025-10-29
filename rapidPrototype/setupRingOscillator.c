
/**************************************************************************

  Name: setupRingOscillator

  Purpose: The purpose of this function is to configure and start up the
  ring oscillator in an R82xx tuner device.

  Calling Sequence: status = setupRingOscillator(priv,
                                                 n_ring,
                                                 out[utDivider,
                                                 outputGain)

  Inputs:

    priv - A pointer to a structure that represents device state.

    n_ring - The divider value for the ring oscillator phase locked
    loop. Valid values are 0 through 15, although, it is reecommended
    to use values between 9 and 14, inclusive.

    outputDivider - A parameter that is used to divide the ring
    oscillator VCO frequency down to a lower value.  Valid values
    are {4,6,8,12,16,24,32,48}.

    outputGain - A parameter that specifies the gain for which the
    ring oscillator output is amplified.  Valid values are
    {-5,0,3,8}.  The units are in decibels.

  Outputs:

    status - The status of the operation. A value of 0 implies success,
    and a value of -1 implies failure.

**************************************************************************/
static int setupRingOscillator(struct r82xx_priv *priv,
                               uint8_t n_ring,
                               uint8_t outputDivider,
                               int outputGain)
{
  int rc;
  uint8_t ring_div_index;
  uint32_t ring_freq;
  uint32_t ring_ref;
  uint8_t gainBits;
  static const uint8_t divBit21[] = {0,0,1,1,2,2,3,3};
  static const uint8_t divBit0[] = {0,0x20,0,0x20,0,0x20,0,0x20};

  if (n_ring > 15)
  {
    // Clip it.
    n_ring = 15;
  } // if

  switch (outputDivider)
  {
    case 4:
    {
      ring_div_index = 0;
      break;
    } // case

    case 6:
    {
      ring_div_index = 1;
      break;
    } // case

    case 8:
    {
      ring_div_index = 2;
      break;
    } // case

    case 12:
    {
      ring_div_index = 3;
      break;
    } // case

    case 16:
    {
      ring_div_index = 4;
      break;
    } // case

    case 24:
    {
      ring_div_index = 5;
      break;
    } // case

    case 32:
    {
      ring_div_index = 6;
      break;
    } // case

    case 48:
    {
      ring_div_index = 7;
      break;
    } // case

    default:
    {
      // Default to divide by 4.
      ring_div_index = 0;
      break;
    } // case

  } // switch

  switch (outputGain)
  {
    case -5:
    {
      gainBits = 0;
      break;
    } // case

    case 0:
    {
      gainBits = 1;
      break;
    } // case

    case 3:
    {
      gainBits = 2;
      break;
    } // case

    case 8:
    {
      gainBits = 3;
      break;
    } // case

    default:
    {
      // Default to -5dB.
      gainBits = 0;
      break;
    } // case

  } // switch

  //  Vco reference is 14.4MHz.
  ring_ref = 14400000;

  // Apply max power to VCO PLL.
  rc = r82xx_write_reg_mask(priv, 0x19, 0x0c, 0x0c);

  // Apply power to ring PLL.
  rc = r82xx_write_reg_mask(priv, 0x18, 0x10, 0x10);

  // Turn on PLL reference clock.
  rc = r82xx_write_reg_mask(priv, 0x0f, 0x00, 0x08);

  // Set N for PLL.
  rc = r82xx_write_reg_mask(priv, 0x18, n_ring, 0x0f);

  // Set divisor.
  rc = r82xx_write_reg_mask(priv, 0x18, divBit0[ring_div_index], 0x20);
  rc = r82xx_write_reg_mask(priv, 0x19, divBit21[ring_div_index], 0x03);

  // Set utput gain in dB.
  rc = r82xx_write_reg_mask(priv, 0x1f, gainBits, 0x03);

  // Select mixer input sources to  ring oscillator.
  rc = r82xx_write_reg_mask(priv, 0x1c, 0x02 , 0x02);

  // Turn off LNA.
  rc = r82xx_write_reg_mask(priv, 0x05, 0xa0, 0xff);

  ring_freq = ((16 + n_ring) * 8) * ring_ref / outputDivider;

  fprintf(stderr,"ring Oscillator Frequency: %u\n",ring_freq);
  
  return (rc);

} // setupRingOscillator


