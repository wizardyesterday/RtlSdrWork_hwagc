
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


