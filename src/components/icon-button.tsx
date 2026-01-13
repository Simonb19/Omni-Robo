import './icon-button.css';
import type { IconButtonProps } from '../types/component-types.ts';

function IconButton({
  icon: Icon,
  size = 32,
  className,
  ...props
}: IconButtonProps) {
  return (
    <button className={`icon-button ${className}`} {...props}>
      <Icon size={size} color={`#fff`} />
    </button>
  );
}

export default IconButton;
